/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2023 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 /**
  * @brief Brief description of this source.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/memdb/simpletable.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>
 #include <udjat/tools/file.h>
 #include <udjat/tools/string.h>
 #include <regex>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <string>
 #include <udjat/tools/memdb/datastore.h>
 #include <fstream>
 #include <set>

 using namespace std;

 namespace Udjat {

	static void split(const std::string &string, std::vector<String> &columns, const char delimiter = ';') {

		columns.clear();
		const char *ptr = string.c_str();

		while(*ptr) {

			if(*ptr == '\"') {

				// It's an string delimited by "
				ptr++;
				const char *to = strchr(ptr,'\"');
				if(!to) {
					throw runtime_error("Bad file, mismatch on '\"' delimiter");
				}
				columns.push_back(std::string{ptr,(size_t) (to-ptr)});
				ptr = strchr(to,delimiter);
				if(!ptr) {
					return;
				}
				ptr++;
			} else {
				const char *to = strchr(ptr,';');
				if(!to) {
					columns.emplace_back(ptr);
					return;
				}

				columns.push_back(std::string{ptr,(size_t) (to-ptr)});
				ptr = to+1;

			}

			while(*ptr && isspace(*ptr)) {
				ptr++;
			}

		}
	}

	void MemCachedDB::Table::load() {

		if(state() == Loading) {
			Logger::String{"Table is already loading, ignoring request"}.warning(name);
			return;
		}

		Logger::String("Loading ",path).trace(name);
		state(Loading);

		// Load input files.
		struct InputFile {
			string name;
			struct stat st;
		};
		std::vector<InputFile> files;

		auto pattern = std::regex(filespec,std::regex::icase);
		Udjat::File::Path{path}.for_each([this,&files,&pattern](const Udjat::File::Path &file){

			if(std::regex_match(file.name(),pattern)) {

				InputFile ifile;

				if(stat(file.c_str(),&ifile.st)) {
					Logger::String{file.c_str(),": ",strerror(errno)}.error(name);
				} else {
					ifile.name = file;
					files.push_back(ifile);
				}

			} else if(Logger::enabled(Logger::Trace)) {

				Logger::String{"Ignoring '",file.c_str(),"'"}.trace(name);

			}

			return false;
		});

		if(files.empty()) {
			Logger::String{"No files to import"}.warning(name);
			file.reset();
			state(Empty);
			return;
		}

		Logger::String{files.size()," file(s) to verify"}.info(name);

		// TODO: If tempfile exists, check if it really need an update.

		//
		// Load files
		//
#ifdef DEBUG
		std::shared_ptr<MemCachedDB::File> file{make_shared<MemCachedDB::File>("/tmp/test.db")};
#else
		std::shared_ptr<MemCachedDB::File> file{make_shared<MemCachedDB::File>()};
#endif // DEBUG

		// Write header
		file->write("\0",1);

		// Write column names.
		for(auto &c : columns) {
			file->write(c->name());
		}
		file->write("\0",1);

		// Write file sources.
		for(auto &f : files) {
			file->write(&f.st.st_mtim,sizeof(f.st.st_mtim));
			file->write(f.name.c_str(),f.name.size()+1);
		}
		file->write("\0",1);

		// Create datastore to avoid string duplication.
		DataStore datastore{file};

		// Create primary index
		// https://stackoverflow.com/questions/14896032/c11-stdset-lambda-comparison-function
		class IndexEntry {
		public:
			size_t *data;

			IndexEntry(const MemCachedDB::Table &table) : data{new size_t[table.columns.size()]} {
				for(size_t ix = 0; ix < table.columns.size(); ix++) {
					data[ix] = 0;
				}
			}

			~IndexEntry() {
				delete[] data;
			}

		};

		// https://stackoverflow.com/questions/14896032/c11-stdset-lambda-comparison-function
		auto comp = [this,file](const IndexEntry &l, const IndexEntry &h){

			// TODO: Compare index columns to check if 'l < h'


			return false;
		};

		/// @brief Ordered set with the records.
		auto index = std::set<IndexEntry,decltype(comp)>( comp );

		// Import CSV files
		for(auto &f : files) {

			Logger::String{"Loading ",f.name.c_str()}.info(name);
			size_t lines = 0;

			std::ifstream infile{f.name};

			String line;
			std::vector<String> headers;


			// Read first line to get field names.
			{
				std::getline(infile, line);
				debug("Header: '",line,"'");

				// TODO: Parse header.
				split(line.strip(), headers,column_separator);

				//debug("Headers:");
				//for(auto &header : headers) {
				//	cout << header << endl;
				//}

			}

			// Map DB columns to CSV columns
			struct Map {
				size_t db;
				size_t csv;
				constexpr Map(size_t d, size_t c) : db{d}, csv{c} {
				}
			};
			std::vector<Map> map;

			for(size_t db = 0; db < columns.size(); db++) {

				debug("Searching for column '",columns[db]->name(),"'");

				for(size_t f = 0; f < headers.size(); f++) {
					if(!strcasecmp(columns[db]->name(),headers[f].c_str())) {
						map.emplace_back(db, f);
						break;
					}
				}

			}

			// Read data
			while(std::getline(infile, line)) {

				line.strip();
				if(line.empty()) {
					Logger::String{"Stopping on empty line '",lines,"'"}.info(name);
					break;
				}

				lines++;

				std::vector<String> cols;
				split(line.strip(), cols,column_separator);

				// Clear input record
				IndexEntry record{*this};

				// Parse fields
				for(const auto &item : map) {
					auto column{columns[item.db]};
					record.data[item.db] = column->store(datastore, cols[item.csv].strip().c_str());
				}

				// Search


			}

			Logger::String{"Got ",f.name.c_str()," with ", lines, " line(s)"};

		}

	}

 }


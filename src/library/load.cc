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
  * @brief Implements datastore loader.
  */
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/loader.h>
 #include <udjat/tools/file.h>
 #include <udjat/tools/logger.h>
 #include <private/structs.h>
 #include <regex>
 #include <set>
 #include <udjat/tools/datastore/column.h>

 using namespace std;

 namespace Udjat {

	DataStore::Loader::Abstract::Abstract(DataStore::Container &c, const char *path, const char *filespec) : container{c} {

		container.state(Updating);

		//
		// Get list of files to import.
		//
		auto pattern = std::regex(filespec,std::regex::icase);
		Udjat::File::Path{path}.for_each([this,&pattern](const Udjat::File::Path &file){

			if(std::regex_match(file.name(),pattern)) {

				InputFile ifile;

				if(stat(file.c_str(),&ifile.st)) {
					Logger::String{file.c_str(),": ",strerror(errno)}.error("DataStore");
				} else {
					ifile.name = file;
					files.push_back(ifile);
				}

			} else if(Logger::enabled(Logger::Trace)) {

				Logger::String{"Ignoring '",file.c_str(),"'"}.trace("DataStore");

			}

			return false;
		});

	}

	shared_ptr<DataStore::File> DataStore::Loader::Abstract::load() {

#ifdef DEBUG
		shared_ptr<File> file{make_shared<File>("/tmp/test.db")};
#else
		shared_ptr<File> file{make_shared<File>()};
#endif // DEBUG

		if(file->size()) {
			throw runtime_error("Datastore is not empty");
		}

		// Build a deduplicator for file.
		Deduplicator dedup{file};

		// Write empty header.
		DataStore::Header header;
		memset(&header,0,sizeof(header));
		header.updated = time(0);
		file->write(header);

		// file->write(&header,sizeof(header));

		// Write file sources.
		for(auto &f : files) {
			file->write(&f.st.st_mtim,sizeof(f.st.st_mtim));
			file->write(f.name.c_str(),f.name.size()+1);
		}
		file->write("\0",1);

		// Create primary index
		// https://stackoverflow.com/questions/14896032/c11-stdset-lambda-comparison-function
		class IndexEntry {
		public:
			size_t length = 0;
			size_t *data = nullptr;

			// Copy constructor.
			IndexEntry(const IndexEntry &src) : length{src.length}, data{new size_t[length]} {
				for(size_t ix = 0; ix < length; ix++) {
					data[ix] = src.data[ix];
				}
			}

			// Copy constructor from pointer.
			IndexEntry(const IndexEntry *src) : length{src->length}, data{new size_t[length]} {
				for(size_t ix = 0; ix < length; ix++) {
					data[ix] = src->data[ix];
				}
			}

			// Standard constructor.
			IndexEntry(size_t columns) : length{columns}, data{new size_t[length]} {
				for(size_t ix = 0; ix < length; ix++) {
					data[ix] = 0;
				}
			}

			~IndexEntry() {
				delete[] data;
				data = nullptr;
			}

		};

		// https://stackoverflow.com/questions/14896032/c11-stdset-lambda-comparison-function
		auto comp = [this,file](const IndexEntry &l, const IndexEntry &h){

			// Compare index columns to check if 'l < h'
			for(size_t col = 0; col < container.columns().size(); col++) {

				if(container.columns()[col]->key() && l.data[col] != h.data[col]) {

					// Not the same vale, compare.
					size_t length = container.columns()[col]->length();
					if(length) {

						// Load real data from file.
						uint8_t lval[length];
						uint8_t hval[length];

						file->read(l.data[col],lval,length);
						file->read(h.data[col],hval,length);

						return container.columns()[col]->comp(lval,hval);

					} else {

						// TODO: Test string.
						return false;
					}

				}

			}

			return false;
		};

		/// @brief Ordered set with the records.
		auto index = std::set<IndexEntry,decltype(comp)>( comp );

		/// @brief Loader context.
		class Context : public DataStore::Loader::Abstract::Context {
		private:
			const Container &container;
			set<IndexEntry,decltype(comp)> &index;
			Deduplicator &deduplicator;
			// vector<shared_ptr<DataStore::Abstract::Column>> columns;

			struct Map {
				size_t from;
				size_t to;
				constexpr Map(size_t f, size_t t) : from{f}, to{t} {
				}
			};
			std::vector<Map> map;

		public:
			Context(const Container &c, std::set<IndexEntry,decltype(comp)> &i, Deduplicator &d) : container{c}, index{i}, deduplicator{d} {
			}

			void open(const std::vector<String> &fromcols) override {

				debug("Headers:");
				for(size_t from = 0; from < fromcols.size(); from++) {
					size_t to = container.column_index(fromcols[from].c_str());
					if(to != ((size_t) -1)) {
						debug("   ",fromcols[from].c_str(),": ",from,"->",to);
						map.emplace_back(from,to);
					}
				}

			}

			void append(std::vector<String> &values) override {

				auto &tocols{container.columns()};
				IndexEntry record{tocols.size()};

				// Parse fields
				for(const auto &item : map) {
					record.data[item.to] = tocols[item.to]->store(deduplicator, values[item.from].strip().c_str());
				}

				// Search
				auto idata = index.find(record);
				if(idata == index.end()) {

					// New record, insert it
					index.insert(record);

				} else {

					// Already exist, update id
//					debug("Record already exist, updating")
					for(const auto &item : map) {
						if(!tocols[item.to]->key()) {
							// It's not a primary key, copy it.
							idata->data[item.to] = record.data[item.to];
						}
					}
				}
			}

		};

		// Load files.
		{
			for(auto &f : files) {
				Logger::String{"Loading ",f.name.c_str()}.info("datastore");
				Context context{container, index, dedup};
				load_file(context,f.name.c_str());
			}
		}

		// Write primary index.
		{
			size_t qtdrec = index.size();
			header.primary_offset = file->write(qtdrec);

			for(const auto &it : index) {
				file->write(it.data,it.length * sizeof(it.data[0]));
//#ifdef DEBUG
//				{
//					const size_t *cptr = it.data;
//					for(const auto &column : container.columns()) {
//						if(!*cptr) {
//							cout << "null";
//						} else if(column->length()) {
//							uint8_t buffer[column->length()];
//							file->read(*cptr,buffer,column->length());
//							cout << column->to_string(buffer);
//						} else {
//							cout << file->read(*cptr);
//						}
//						cout << " ";
//						cptr++;
//					}
//					cout << endl;
//				}
//#endif // DEBUG

			}

		}

		// Build & write extended indexes.
		{

		}

		// Write updated header
		file->write(0, header);

		// Return new data storage.
		debug("Records: ",index.size());
		return file;

	}

 }


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
 #include <udjat/tools/threadpool.h>
 #include <udjat/tools/quark.h>
 #include <udjat/tools/memdb/simpletable.h>
 #include <private/loader.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>
 #include <udjat/tools/object.h>
 #include <udjat/tools/file.h>
 #include <regex>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <string>

 using namespace std;

 namespace Udjat {

	#pragma pack(1)
	struct Header {
		uint16_t sources;	///< @brief Number of import files
	};
	#pragma pack()

	MemCachedDB::Table::Table(const XML::Node &definition)
		: name{Quark{definition,"name","",false}.c_str()},
			path{Object::getAttribute(definition,"path","")},
			filespec{Object::getAttribute(definition,"filespec",".*")} {

		if(!*name) {
			throw runtime_error("Required attribute 'name' is missing");
		}

		if(!*path) {
			throw runtime_error("Required attribute 'path' is missing");
		}

		for(XML::Node child = definition.child("column"); child; child = child.next_sibling("column")) {

			const char *type = child.attribute("type").as_string("string");

			if(!strcasecmp(type,"int")) {
				columns.emplace_back(make_shared<Column<int>>(child));
			} else if(!strcasecmp(type,"uint")) {
				columns.emplace_back(make_shared<Column<unsigned int>>(child));
			} else if(!strcasecmp(type,"string")) {
				columns.emplace_back(make_shared<Column<std::string>>(child));
			} else {
				throw runtime_error(Logger::String{"Unexpected column type: ",type});
			}

		}

	}

	MemCachedDB::Table::~Table() {
	}

	void MemCachedDB::Table::load() {

		debug("-----------------------------------------------------")

		if(state() == Loading) {
			Logger::String{"Table is already loading, ignoring request"}.warning(name);
			return;
		}

		Logger::String("Loading ",path).trace(name);
		state(Loading);

		debug("filespec=",filespec);
		auto pattern = std::regex(filespec,std::regex::icase);

		struct InputFile {
			string name;
			struct stat st;
		};
		std::vector<InputFile> files;
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

		Header hdr;
		memset(&hdr,0,sizeof(hdr));

		hdr.sources = (uint16_t) files.size();

		// Write header
		file->write(&hdr,sizeof(hdr));

		// Write file sources.
		for(auto &f : files) {
			file->write(&f.st.st_mtim,sizeof(f.st.st_mtim));
			file->write(f.name.c_str(),f.name.size()+1);
		}

		// Do CSV loading.

	}

 }


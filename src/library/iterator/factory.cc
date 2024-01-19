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
  * @brief Implements container iterator.
  */

 #include <config.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/logger.h>
 #include <private/structs.h>

 using namespace std;

 namespace Udjat {

	DataStore::Iterator DataStore::Iterator::Factory(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const char *path) {

		debug(__FUNCTION__,"('",path,"')");

		while(*path && *path == '/') {
			path++;
		}

		// Build an agnostic iterator.
		Iterator it{file,cols};

		// Check for column filter.
		{
			size_t szpath = strlen(path);

			for(size_t ix = 0; ix < cols.size(); ix++) {

				if(!cols[ix]->indexed()) {
					continue;
				}

				const char *name = cols[ix]->name();
				size_t szname = strlen(name);

				debug(name," - ",path," ",szname);

				if(szpath > szname && path[szname] == '/' && !strncasecmp(path,name,szname)) {

					debug("Using column '",name,"' as index");
					path += szname + 1;
					debug("Path updated to '",path,"'");

					// Set column.
					it.ixtype = (uint16_t) ix;

					// Get pointer to ixtype index.
					const Header &header{file->get<Header>(0)};
					const Index *index{file->get_ptr<Index>(header.indexes.offset)};

					it.ixptr = nullptr;
					for(size_t ix = 0;ix < header.indexes.count;ix++) {

						if(index->column == it.ixtype) {
							it.ixptr = file->get_ptr<size_t>(index->offset);
							break;
						}
						index++;
					}

					if(!it.ixptr) {
						throw logic_error(Logger::String{"Unable to find index for '",name,"'"});
					}

					it = 0; // First row.

					break;
				}


			}

		}

		// Check for row filter.
		if(!strncasecmp(path,"row/",4)) {

			// Select single row.
			size_t row = stoi(path+4);
			it = row;

			it.filter = [row](const Iterator &it) {
				return it.row == row ? 0 : 1;
			};

			return it;
		}

		// Use remaining path as filter key.
		it.set_default_filter(path);

		return it;
	}

	DataStore::Iterator DataStore::Iterator::Factory(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const Request &request) {
		return Factory(file,cols,request.path());
	}


	/*
	DataStore::Iterator DataStore::Iterator::Factory(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const char *path) {

		while(*path && *path == '/') {
			path++;
		}

		const char *mark = strchr(path,'/');

		if(!mark) {
			// Primary key search.
			return DataStore::Iterator{file,cols,path};
		}

		mark++;

		return DataStore::Iterator{file,cols,string{path,(size_t) (mark - path)-1},mark};

	}
	*/

 }

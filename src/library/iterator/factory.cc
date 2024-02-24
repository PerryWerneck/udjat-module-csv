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
 #include <private/iterator.h>
 #include <udjat/tools/string.h>

 using namespace std;

 namespace Udjat {

	static uint16_t get_column_id(const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const char *name) {

		debug("Searching for column '",name,"'");

		for(size_t col = 0; col < cols.size(); col++) {
			if(!strcasecmp(cols[col]->name(),name)) {
				return col;
			}
		}

		return (uint16_t) -1;
	}

	DataStore::Iterator DataStore::Iterator::Factory(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const char *path) {

		debug(__FUNCTION__,"('",path,"')");

		while(*path && *path == '/') {
			path++;
		}

		// Build an agnostic iterator.
		Iterator it{file,cols};

		// Check for column filter.
		uint16_t column_id = (uint16_t) -1;
		{
			const char *ptr = strchr(path,'/');
			if(ptr) {
				// Column as part of name.
				column_id = get_column_id(cols,string{path,(size_t) (ptr-path)}.c_str());

				if(column_id != (uint16_t) -1) {
					if(cols[column_id]->indexed()) {
						it.handler = make_shared<ColumnKeyHandler>(it,column_id);
					}
					path = ptr+1;
				}
			}
		}

		// It there's no handler, use the default one.
		if(!it.handler) {
			it.handler = make_shared<PrimaryKeyHandler>(it);
		}

		// Check for row filter.
		if(!strncasecmp(path,"row/",4)) {

			// Select single row.
			size_t row = stoi(path+4);
			it = row;

			class Handler : public PrimaryKeyHandler {
			private:
				size_t selected_row;

			public:
				Handler(const Iterator &it, size_t r) : PrimaryKeyHandler{it}, selected_row{r} {
				}

				int filter(const Iterator &it) const override {
					return row(it) == selected_row ? 0 : 1;
				}

			};

			it.handler = make_shared<Handler>(it,row);

			return it;
		}

		if(!strncasecmp(path,"contains/",9)) {

			// Search for partial content.
			it = 0;
			path += 9;

			auto records = make_shared<CustomKeyHandler>();

			if(column_id != (uint16_t) -1) {

				// Search for column contents
				debug("Searching for substring '",path,"' on column '",cols[column_id]->name());

				for(size_t row = 0; row < it.handler->size(); row++) {
					it = row;
					if(String::strcasestr(it[(size_t) column_id].c_str(),path)) {
						debug("Selecting '",it[(size_t) column_id],"' rowptr=", ((size_t) it.rowptr())," size=");
						records->push_back(it);
					}
				}

			} else {

				// Search on all columns
				debug("Searching for substring '",path,"' on all columns");

				for(size_t row = 0; row < it.handler->size(); row++) {
					for(size_t ix = 0;ix < cols.size();ix++) {
						if(String::strcasestr(it[(size_t) ix].c_str(),path)) {
							debug("Selecting '",it[(size_t) ix],"' rowptr=", ((size_t) it.rowptr()));
							records->push_back(it);
							break;
						}
					}
					it++;
				}

			}

			debug("Got ",records->size()," records");
			it.handler = records;
			it = 0;

			return it;
		}

		// Use remaining path as search key.
		it.handler->key(path);
		it.search();

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

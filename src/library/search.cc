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
  * @brief Search resource from similar string.
  */

 #include <config.h>
 #include <config.h>
 #include <udjat/tools/datastore/container.h>
 #include <udjat/tools/logger.h>
 #include <private/structs.h>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	using Container = DataStore::Container;

	Container::Iterator get_path(const DataStore::Container &container, const char *path) {

		while(*path && *path == '/') {
			path++;
		}

		if(!strncasecmp(path,"row/",4)) {
			// Search by path.
			Container::Iterator it{container.begin()};
			it += atoi(path+4);
			return it;
		}

		const char *mark = strchr(path,'/');

		if(!mark) {
			// Primary key search.
			Container::Iterator it{container.begin()};
			it.find(path);
			return it;
		}

		mark++;

		Container::Iterator it{container.begin(string{path,(size_t) (mark - path)-1}.c_str())};
		it.find(mark);
		return it;

	}

	bool DataStore::Container::get(const char *path, Udjat::Value &value) const {

		// It's asking for a single row??
		if(strncasecmp(path,"row/",4) == 0) {
			Container::Iterator it{begin()};
			it.set(atoi(path+4)-1);
			return it;
		}

		Container::Iterator it{get_path(*this,path)};

		if(it) {
			it.get(value);
			return true;
		}

		return false;

	}

	bool DataStore::Container::get(const char *path, Udjat::Response::Value &value) const {

		Container::Iterator it{get_path(*this,path)};

		if(it) {
			// TODO: Set value for last_modified()
			it.get(value);
			return true;
		}

		return false;

	}

	bool DataStore::Container::get(const char *path, Udjat::Response::Table &value) const {

		Container::Iterator it{get_path(*this,path)};
		if(!it) {
			return false;
		}

		// Todo: Setup last_modified()

		// Start report
		std::vector<std::string> column_names;

		for(auto col : cols) {
			column_names.push_back(col->name());
		}

		value.start(column_names);

		while(it) {

			for(auto col : column_names) {
				value.push_back(it[col.c_str()]);
			}

			it++;
		}

		return true;

	}

	int DataStore::Container::Iterator::compare(const char *key) const {

		const size_t *row = rowptr();

		if(filter.column == (uint16_t) -1) {

			// Compare using primary key

			for(auto col : cols) {

				if(col->key()) {

					// It's a primary column, test it.
					string value{col->to_string(file,row)};

					debug("col: '",value,"' key: '",key,"'");

					size_t keylen = strlen(key);

					if(keylen < value.size()) {

						// The query string is smaller than the column, do a partial test.
						return strncasecmp(value.c_str(),key,keylen);

					} else {

						// The query string is equal or larger than the column, do a full test.
						int rc = strncasecmp(value.c_str(),key,value.size());
						if(rc) {
							return rc;
						}

					}

					// Get next block.
					key += value.size();
					if(!*key) {
						// Key is complete, found it.
						return 0;
					}

				}
			}

			return 1;

		}

		// Compare using only the selected column.
		//return cols[filter.column]->comp(file,ptr[filter.column],key);
		return 1;

	}

	DataStore::Container::Iterator & DataStore::Container::Iterator::find(const char *key) {

		if(!(key && *key)) {
			set(1);
			return *this;
		}

		filter.key = key;

		size_t from = 0;
		size_t to = ixptr[0];

		while( (to - from) > 1 ) {

			row = from+((to-from)/2);
			debug("Center row=",row," from=",from," to=",to, " to-from=",(to-from));

			int comp{compare(key)};

			debug("comp=",comp);

			if(comp == 0) {

				// Found!

				debug("Found ",key," at row ",row," (",to_string(),")");

				// Go down until the first occurrence (Just in case).
				size_t first_row = row;
				while(row > 1) {
					row--;
					if(compare(key)) {
						break;
					} else {
						first_row = row;
					}
				}
				row = first_row;

				debug("Final result was ",row," (",to_string(),")");

				return *this;

			} else if(comp < 0) {

				// Current is lower, get highest values
				from = row;

			} else {

				// Current is bigger, get lower values
				to = row;

			}

		}

		debug("Can't find '",key,"'");
		row = ixptr[0];

		return *this;
	}

 }


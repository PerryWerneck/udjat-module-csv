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
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	bool DataStore::Container::get(const char *path, Udjat::Value &value) const {

		while(*path && *path == '/') {
			path++;
		}

		const char *mark = strchr(path,'/');

		if(!mark) {

			// Primary key search.
			Iterator it{find(path)};

			if(it) {
				debug("Found resource '",it.to_string(),"'");
				it.get(value);
				return true;
			}

			return false;

		}

		mark++;

		if(strncasecmp(path,"rownumber/",10) == 0) {

			Iterator it{begin()};

			it.set(atoi(path+10)-1);

			if(it) {
				it.get(value);
				return true;
			}

		}

		return false;

	}

	int DataStore::Container::Iterator::compare(const char *key) const {

		const size_t *ptr = rowptr();

		for(auto col : cols) {

			if(col->key()) {

				// It's a primary column, test it.
				string value{col->to_string(file,*ptr)};
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
			ptr++;
		}

		return 1;

	}

	DataStore::Container::Iterator & DataStore::Container::Iterator::search(const char *key) {

		size_t from = 0;
		size_t to = ixptr[0];

		debug("------------------------------Searching for '",key,"'");

		while( (to - from) > 1 ) {

			row = from+((to-from)/2);
			debug("Center row=",row);

			if(*this == key) {

				debug("Found ",key," at row ",row," (",to_string(),")");

				// Go down until the first occurrence (Just in case).
				size_t first_row = row;
				while(row > 1) {
					row--;
					if(*this == row) {
						first_row = row;
					} else {
						break;
					}
				}
				row = first_row;

				debug("Final result was ",row," (",to_string(),")");

				return *this;

			} else {

				int comp{compare(key)};

				debug("comp=",comp," (",to_string(),")");

				if(comp < 0) {

					// Current is lower, get highest values
					from = row;

				} else if(comp > 0) {

					// Current is bigger, get lower values
					to = row;

				} else {

					throw logic_error(Logger::String{"Internal error on ",__FILE__,"(",__LINE__,")"});

				}

			}

			debug("from=",from," to=",to, " to-from=",(to-from));

		}

		debug("--------------------------Can't find '",key,"'");
		row = ixptr[0];

		return *this;
	}

 }


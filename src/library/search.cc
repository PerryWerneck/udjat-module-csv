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
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	int DataStore::Container::Resource::compare(const char *key) const {

		const size_t *ptr = recptr();

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

	DataStore::Container::Resource & DataStore::Container::Resource::search(const char *key) {

		/*
		size_t from = 0;
		size_t to = ixptr[0];

		debug("Searching for '",key,"'");

		while( (to - from) > 1 ) {

			index = from+((to-from)/2);
			debug("Center rec=",index);

			if(*this == key) {
				debug("Found ",key," at index ",index);
				// FIXME: Find first line.
				return *this;
			}



		}
		*/

		index = ixptr[0];
		return *this;
	}

 }


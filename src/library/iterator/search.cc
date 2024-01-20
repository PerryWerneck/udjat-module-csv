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
  * @brief Implement iterator search for key.
  */

 #include <config.h>

 #include <config.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/logger.h>
 #include <private/iterator.h>

 using namespace std;

 namespace Udjat {

	void DataStore::Iterator::search() {

		size_t from = 0;
		size_t to = handler->size();

		while( (to - from) > 1 ) {

			row = from+((to-from)/2);
			debug("Center row=",row," from=",from," to=",to, " to-from=",(to-from));

			int comp{handler->filter(*this)};

			debug("comp=",comp);

			if(comp == 0) {

				// Found!

				debug("Found ",primary_key()," at row ",row);

				// Go down until the first occurrence (Just in case).
				size_t first_row = row;
				while(row > 1) {
					row--;
					if(handler->filter(*this)) {
						break;
					} else {
						first_row = row;
					}
				}
				row = first_row;

				debug("Final result was ",primary_key()," at row ",row);

				return;

			} else if(comp < 0) {

				// Current is lower, get highest values
				from = row;

			} else {

				// Current is bigger, get lower values
				to = row;

			}

		}

		debug(__FUNCTION__," has failed");
		row = handler->size();

	}

 }

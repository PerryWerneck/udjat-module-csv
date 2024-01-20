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
  * @brief Implements iterator handler.
  */

 #include <config.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/datastore/file.h>
 #include <private/iterator.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>
 #include <private/structs.h>

 using namespace std;

 namespace Udjat {

	DataStore::ColumnKeyHandler::ColumnKeyHandler(const Iterator &it, uint16_t c) : PrimaryKeyHandler{it}, colnumber{c} {

		// Get pointer to column index.
		const Header &header{file(it)->get<Header>(0)};
		const Index *index{file(it)->get_ptr<Index>(header.indexes.offset)};

		ixptr = nullptr;
		for(size_t ix = 0;ix < header.indexes.count;ix++) {

			if(index->column == colnumber) {
				ixptr = file(it)->get_ptr<size_t>(index->offset);
				break;
			}
			index++;
		}

		if(!ixptr) {
			throw logic_error(Logger::String{"Unable to find index for required column"});
		}

	}

	size_t DataStore::PrimaryKeyHandler::size() const {
		return ixptr[0];
	}

	void DataStore::PrimaryKeyHandler::key(const char *key) {
		search_key = key;
	}


	const size_t * DataStore::PrimaryKeyHandler::rowptr(const Iterator &it) const {

		if(row(it) > ixptr[0]) {
			throw runtime_error(Logger::String{"Invalid row, should be from 0 to ",(int) ixptr[0]});
		}

		return ixptr + 1 + (row(it) * cols(it).size());

	}

	const size_t * DataStore::ColumnKeyHandler::rowptr(const Iterator &it) const {

		if(row(it) > ixptr[0]) {
			throw runtime_error(Logger::String{"Invalid row, should be from 0 to ",(int) ixptr[0]});
		}

		return file(it)->get_ptr<size_t>( *(ixptr + 1 + row(it)) );

	}

	/*
	void DataStore::Iterator::set_default_index(const std::string &search_key) {

		if(ixtype != (uint16_t) -1) {

			// Use column based filter.
			filter = [search_key](const Iterator &it) {
				return it.cols[it.ixtype]->comp(it.file,it.rowptr(),search_key.c_str());
			};


		} else {

			// Use primary key filter.
			filter = [search_key](const Iterator &it) {

				const char *key = search_key.c_str();

				const size_t *row{it.rowptr()};
				for(const auto col : it.cols) {

					if(col->key()) {

						// It's a primary column, test it.
						string value{col->to_string(it.file,row)};
						col->apply_layout(value);

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

			};

		}

		search();	// Select first entry.
	}
	*/

 }

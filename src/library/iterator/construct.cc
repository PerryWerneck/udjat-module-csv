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
 #include <udjat/tools/datastore/file.h>
 #include <udjat/tools/logger.h>
 #include <private/structs.h>

 using namespace std;

 namespace Udjat {

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c)
		: file{f}, cols{c}, filter{[](const Iterator &){return 0;}} {

		if(!file->mapped()) {
			throw logic_error("Unable to iterate an unmapped datastore");
		}

		// Get pointer to primary index.
		ixptr = file->get_ptr<size_t>(file->get<Header>(0).primary_offset);

	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const std::string &search_key)
		: file{f}, cols{c} {

		if(!file->mapped()) {
			throw logic_error("Unable to iterate an unmapped datastore");
		}

		// Get pointer to primary index.
		ixptr = file->get_ptr<size_t>(file->get<Header>(0).primary_offset);

		// Filter using primary keys.
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

		search();

	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const std::string &column_name, const std::string &search_key)
		: file{f}, cols{c} {

		if(!file->mapped()) {
			throw logic_error("Unable to iterate an unmapped datastore");
		}

		// Get column index.
		ixtype = (uint16_t) -1;
		for(size_t c = 0; c < cols.size();c++) {
			if(cols[c]->indexed() && !strcasecmp(cols[c]->name(),column_name.c_str())) {
				ixtype = (uint16_t) c;
				break;
			}
		}
		if(ixtype == (uint16_t) -1) {
			throw runtime_error(Logger::String{"Unable to search using '",column_name.c_str(),"'"});
		}

		// Get pointer to ixtype index.
		const Header &header{file->get<Header>(0)};
		const Index *index{file->get_ptr<Index>(header.indexes.offset)};

		for(size_t ix = 0;ix < header.indexes.count;ix++) {

			if(index->column == ixtype) {
				ixptr = file->get_ptr<size_t>(index->offset);
				break;
			}

			index++;
		}
		if(!ixptr) {
			throw logic_error(Logger::String{"Cant find index for '",column_name,"'"});
		}

		// Construct filter.
		filter = [search_key](const Iterator &it) {
			return it.cols[it.ixtype]->comp(it.file,it.rowptr(),search_key.c_str());
		};

		search();
	}

 }

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
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/logger.h>
 #include <private/structs.h>
 #include <private/iterator.h>

 using namespace std;

 namespace Udjat {

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c)
		: file{f}, cols{c} {

		if(!file->mapped()) {
			throw logic_error("Unable to iterate an unmapped datastore");
		}

	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const std::string &search_key)
		: file{f}, cols{c}, handler{make_shared<DataStore::PrimaryKeyHandler>(*this,search_key.c_str())} {

		search();

	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const std::string &column_name, const std::string &search_key)
		: file{f}, cols{c}, handler{make_shared<ColumnKeyHandler>(*this,column_name.c_str(),search_key.c_str())} {

		search();

		/*
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

		set_default_filter(search_key);
		*/

	}

 }

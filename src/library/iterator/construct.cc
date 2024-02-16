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

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, std::shared_ptr<Handler> h)
		: file{f}, cols{c}, handler{h} {

		if(!file) {
			throw std::system_error(ENODATA,std::system_category());
		}

		if(!file->mapped()) {
			throw logic_error("Unable to iterate an unmapped datastore");
		}
	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c)
		: file{f}, cols{c} {

		if(!file) {
			throw std::system_error(ENODATA,std::system_category());
		}

		if(!file->mapped()) {
			throw logic_error("Unable to iterate an unmapped datastore");
		}
	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const std::string &search_key)
		: Iterator{f,c,make_shared<DataStore::PrimaryKeyHandler>(*this,search_key.c_str())} {
		search();
	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const uint16_t column_id, const std::string &search_key)
		: file{f}, cols{c}, handler{make_shared<ColumnKeyHandler>(*this,column_id,search_key.c_str())} {
		search();
	}

	DataStore::Iterator::Iterator(const std::shared_ptr<DataStore::File> f, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &c, const std::string &column_name, const std::string &search_key)
		: Iterator{f,c,make_shared<ColumnKeyHandler>(*this,column_name.c_str(),search_key.c_str())} {
		search();
	}

 }

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
  * @brief Implements a Iterator inside the container.
  */

 #include <config.h>
 #include <udjat/tools/datastore/container.h>
 #include <udjat/tools/datastore/file.h>
 #include <udjat/tools/datastore/column.h>
 #include <private/structs.h>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	/*
	DataStore::Container::Iterator::Iterator(std::shared_ptr<File> f, const std::vector<std::shared_ptr<Abstract::Column>> &c, size_t i)
		: file{f}, cols{c}, index{i} {

		if(!file) {
			throw std::system_error(ENODATA,std::system_category(),"Empty data store");
		}

		const Header &header{file->get<Header>(0)};

		if(!header.primary_offset) {
			throw std::system_error(ENODATA,std::system_category(),"Empty data store");
		}

		ixptr = (size_t *) file->get_void_ptr(header.primary_offset);

	}
	*/


 }

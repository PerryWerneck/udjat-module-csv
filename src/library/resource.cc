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
  * @brief Implements a resource inside the container.
  */

 #include <config.h>
 #include <udjat/tools/datastore/container.h>
 #include <udjat/tools/datastore/file.h>
 #include <private/structs.h>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	DataStore::Container::Resource::Resource(std::shared_ptr<File> f, const std::vector<std::shared_ptr<Abstract::Column>> &c, size_t i)
		: file{f}, cols{c}, index{i} {

		const Header &header{file->get<Header>(0)};

		if(!header.primary_offset) {
			throw std::system_error(ENODATA,std::system_category(),"Empty data store");
		}

		ixptr = (size_t *) file->get_void_ptr(header.primary_offset);

	}

	size_t DataStore::Container::Resource::count() const {
		return ixptr[0];
	}

	DataStore::Container::Resource & DataStore::Container::Resource::set(size_t id) {
		if(id > ixptr[0]) {
			throw out_of_range("Beyond storage limit");
		}
		index = id;
		return *this;
	}

	DataStore::Container::Resource::operator bool() const {
		return index < ixptr[0];
	}

	DataStore::Container::Resource& DataStore::Container::Resource::operator++() {
		index++;
		if(index > ixptr[0]) {
			index = ixptr[0];
		}
		return *this;
	}

	DataStore::Container::Resource& DataStore::Container::Resource::operator--() {
		if(index == 0) {
			throw out_of_range("Already at the first resource");
		}
		index--;
		return *this;
	}

	DataStore::Container::Resource DataStore::Container::Resource::operator++(int) {
		Resource rc = *this;
		index++;
		if(index > ixptr[0]) {
			index = ixptr[0];
		}
		return rc;
	}

	DataStore::Container::Resource DataStore::Container::Resource::operator--(int) {
		if(index == 0) {
			throw out_of_range("Already at the first resource");
		}
		Resource rc = *this;
		index--;
		return rc;
	}

	bool DataStore::Container::Resource::operator== (const DataStore::Container::Resource& b) {
		return index == b.index && ixptr == b.ixptr;
	}

	bool DataStore::Container::Resource::operator!= (const DataStore::Container::Resource& b) {
		return index != b.index || ixptr != b.ixptr;
	}

	Udjat::Value & DataStore::Container::Resource::get(Udjat::Value &value) const {

		if(index >= ixptr[0]) {
			return value;
		}

		// Get all columns.



		return value;
	}

 }

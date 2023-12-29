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
 #include <udjat/tools/datastore/column.h>
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

	DataStore::Container::Resource::~Resource() {
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
		return !operator==(b);
	}

	bool DataStore::Container::Resource::operator== (const char *key) const {

		const size_t *ptr = recptr();

		for(auto col : cols) {

			if(col->key()) {

				// It's a primary column, test it.

				string value{col->to_string(file,*ptr)};
				size_t keylen = strlen(key);

				if(keylen < value.size()) {

					// The query string is smaller than the column, do a partial test.
					return strncasecmp(value.c_str(),key,keylen) == 0;

				} else {

					// The query string is equal or larger than the column, do a full test.
					if(strncasecmp(value.c_str(),key,value.size())) {
						return false;
					}

				}

				// Get next block.
				key += value.size();
				if(!*key) {
					// Key is complete, found it.
					return true;
				}

			}
			ptr++;
		}

		return false;
	}

	const size_t * DataStore::Container::Resource::recptr() const {
		return ixptr + 1 + (index * cols.size());
	}

	std::string DataStore::Container::Resource::operator[](const char *column) const {

		const size_t *ptr = recptr();
		for(auto col : cols) {
			if(!strcasecmp(column,col->name())) {
				return col->to_string(file,*ptr);
			}
			ptr++;
		}

		return "";
	}

	Udjat::Value & DataStore::Container::Resource::get(Udjat::Value &value) const {

		if(index >= ixptr[0]) {
			return value;
		}

		// Get all columns.
		const size_t *ptr = recptr();
		for(auto col : cols) {
			value[col->name()] = col->to_string(file,*ptr);
			ptr++;
		}

		return value;
	}

 }

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
 #include <udjat/tools/datastore/container.h>
 #include <udjat/tools/datastore/file.h>
 #include <private/structs.h>

 using namespace std;

 namespace Udjat {

	DataStore::Container::Iterator DataStore::Container::begin() const {

		// Standard iterator, uses primary index
		Iterator it{*this};

		const Header &header{active_file->get<Header>(0)};

		if(!header.primary_offset) {
			throw std::system_error(ENODATA,std::system_category(),"Empty data store");
		}

		it.format = 1;
		it.ixptr = active_file->get_ptr<size_t>(header.primary_offset);

		return it;
	}

	DataStore::Container::Iterator DataStore::Container::end() const {

		Iterator it{*this};

		const Header &header{active_file->get<Header>(0)};

		if(!header.primary_offset) {
			throw std::system_error(ENODATA,std::system_category(),"Empty data store");
		}

		it.format = 1;
		it.ixptr = active_file->get_ptr<size_t>(header.primary_offset);
		it.row = it.ixptr[0];	// Last row.

		return it;

	}

	DataStore::Container::Iterator::Iterator(const Container &container)
		: file{container.active_file}, cols{container.cols} {

		if(!file) {
			throw std::system_error(ENODATA,std::system_category(),"Empty data store");
		}

	}

	DataStore::Container::Iterator::~Iterator() {
	}

	size_t DataStore::Container::Iterator::count() const {
		return ixptr[0];
	}

	DataStore::Container::Iterator & DataStore::Container::Iterator::set(size_t id) {
		if(id > ixptr[0]) {
			id = ixptr[0];
		}
		row = id;
		return *this;
	}

	DataStore::Container::Iterator& DataStore::Container::Iterator::operator+=(size_t off) {
		set(row+off);
		return *this;
	}

	DataStore::Container::Iterator DataStore::Container::Iterator::operator+(size_t off) const {
		Iterator rc = *this;
		rc.set(row+off);
		return rc;
	}

	DataStore::Container::Iterator& DataStore::Container::Iterator::operator-=(size_t off) {
		if(off > row) {
			throw out_of_range("Invalid offset");
		}
		set(row-off);
		return *this;
	}

	DataStore::Container::Iterator DataStore::Container::Iterator::operator-(size_t off) const {
		if(off > row) {
			throw out_of_range("Invalid offset");
		}
		Iterator rc = *this;
		rc.set(row-off);
		return rc;
	}

	DataStore::Container::Iterator::operator bool() const {
		return row < ixptr[0];
	}

	DataStore::Container::Iterator& DataStore::Container::Iterator::operator++() {
		row++;
		if(row > ixptr[0]) {
			row = ixptr[0];
		}
		return *this;
	}

	DataStore::Container::Iterator& DataStore::Container::Iterator::operator--() {
		if(row == 0) {
			throw out_of_range("Already at the first Iterator");
		}
		row--;
		return *this;
	}

	DataStore::Container::Iterator DataStore::Container::Iterator::operator++(int) {
		Iterator rc = *this;
		row++;
		if(row > ixptr[0]) {
			row = ixptr[0];
		}
		return rc;
	}

	DataStore::Container::Iterator DataStore::Container::Iterator::operator--(int) {
		if(row == 0) {
			throw out_of_range("Already at the first Iterator");
		}
		Iterator rc = *this;
		row--;
		return rc;
	}

	bool DataStore::Container::Iterator::operator== (const DataStore::Container::Iterator& b) const {
		return row == b.row && ixptr == b.ixptr;
	}

	bool DataStore::Container::Iterator::operator!= (const DataStore::Container::Iterator& b) const {
		return !operator==(b);
	}

	bool DataStore::Container::Iterator::operator< (const Iterator& b) const {
		return row < b.row && ixptr == b.ixptr;
	}

	bool DataStore::Container::Iterator::operator<= (const Iterator& b) const{
		return row <= b.row && ixptr == b.ixptr;
	}

	bool DataStore::Container::Iterator::operator> (const Iterator& b) const{
		return row > b.row && ixptr == b.ixptr;
	}

	bool DataStore::Container::Iterator::operator>= (const Iterator& b) const{
		return row >= b.row && ixptr == b.ixptr;
	}

	const size_t * DataStore::Container::Iterator::rowptr() const {

		throw runtime_error("Incomplete, should consider the type");

		//return ixptr + 1 + (index * cols.size());
	}

	std::string DataStore::Container::Iterator::operator[](const char *name) const {

		throw runtime_error("Needs refactor");

	}

	Udjat::Value & DataStore::Container::Iterator::get(Udjat::Value &value) const {

		if(row >= ixptr[0]) {
			return value;
		}

		// Get all columns.
		const size_t *ptr = rowptr();
		for(auto col : cols) {
			value[col->name()] = col->to_string(file,*ptr);
			ptr++;
		}

		return value;
	}

	std::string DataStore::Container::Iterator::to_string() const {

		std::string rc;
		const size_t *ptr = rowptr();

		for(auto col : cols) {
			if(col->key()) {
				if(!rc.empty()) {
					rc += " ";
				}
				rc += col->to_string(file,*ptr);
			}
			ptr++;
		}
		return rc;

	}

 }

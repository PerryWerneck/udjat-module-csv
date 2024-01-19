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

	DataStore::Iterator & DataStore::Iterator::operator=(const size_t row) {
		if(row > ixptr[0]) {
			this->row = ixptr[0];
		} else {
			this->row = row;
		}
		return *this;
	}

	DataStore::Iterator::operator bool() const {
		if(row > ixptr[0]) {
			return false;
		}
		return filter(*this) == 0;
	}

	DataStore::Iterator & DataStore::Iterator::operator++() {
		row++;
		if(row > ixptr[0]) {
			row = ixptr[0];
		}
		return *this;
	}

	DataStore::Iterator & DataStore::Iterator::operator--() {
		if(row == 0) {
			throw out_of_range("Already at the first Iterator");
		}
		row--;
		return *this;
	}

	DataStore::Iterator DataStore::Iterator::operator++(int) {
		Iterator rc = *this;
		row++;
		if(row > ixptr[0]) {
			row = ixptr[0];
		}
		return rc;
	}

	DataStore::Iterator DataStore::Iterator::operator--(int) {
		if(row == 0) {
			throw out_of_range("Already at the first Iterator");
		}
		Iterator rc = *this;
		row--;
		return rc;
	}


 }

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

	const size_t * DataStore::Iterator::rowptr() const {

		if(row > ixptr[0]) {
			throw runtime_error(Logger::String{"Invalid row, should be from 0 to ",(int) ixptr[0]});
		}

		if(ixtype == (uint16_t) -1) {
			return ixptr + 1 + (row * cols.size());
		}

		return file->get_ptr<size_t>(ixptr[row]);

	}

	std::string DataStore::Iterator::primary_key() const {

		if(row > ixptr[0]) {
			return "";
		}

		std::string rc;
		const size_t * cdata{rowptr()};

		for(const auto col : cols) {

			if(col->key()) {
				string str{col->to_string(file,cdata)};
				rc += col->apply_layout(str);
			}

		}

		return rc;
	}

	std::string DataStore::Iterator::operator[](const char *name) const {

		if(row > ixptr[0]) {
			return "";
		}

		for(auto col : cols) {
			if(!strcasecmp(col->name(),name)) {
				std::string rc{col->to_string(file,rowptr())};
				col->apply_layout(rc);
				return rc;
			}
		}

		Logger::String{"Unexpected column '",name,"', returning empty string"}.warning("datastore");

		return "";
	}

	std::string DataStore::Iterator::operator[](const size_t ix) const {

		if(row > ixptr[0]) {
			return "";
		}

		std::string rc{cols[ix]->to_string(file,rowptr())};
		cols[ix]->apply_layout(rc);
		return rc;

	}

	Udjat::Value & DataStore::Iterator::get(Udjat::Value &value) const {

		if(row > ixptr[0]) {
			return value;
		}

		const size_t *rptr = rowptr();
		for(auto col : cols) {
			std::string rc{col->to_string(file,rptr)};
			col->apply_layout(rc);
			value[col->name()] = rc;
		}

		return value;
	}

 }

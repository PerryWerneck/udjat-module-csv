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
 #include <private/iterator.h>

 using namespace std;

 namespace Udjat {

	const size_t * DataStore::Iterator::rowptr() const {
		return handler->rowptr(*this);
	}

	std::string DataStore::Iterator::primary_key() const {

		if(row > handler->size()) {
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

		if(row > handler->size()) {
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

		if(row > handler->size()) {
			return "";
		}

		std::string rc{cols[ix]->to_string(file,rowptr())};
		cols[ix]->apply_layout(rc);
		return rc;

	}

	Udjat::Value & DataStore::Iterator::get(Udjat::Value &value) const {

		if(row > handler->size()) {
			return value;
		}

		const size_t *row = rowptr();
		for(auto col : cols) {
			col->get(file,row,value);
		}

		return value;
	}

	size_t DataStore::Iterator::count() const {

		size_t rc = 0;

		if(*this) {
			DataStore::Iterator it{*this};
			while(it) {
				rc++;
				it++;
			}
		}

		return rc;
	}

	bool DataStore::Iterator::head(Udjat::Abstract::Response &value) const {

		if(!*this) {
			return false;
		}

		value.last_modified(file->get<Header>(0).last_modified);
		value.count(this->count());

		return true;
	}

	bool DataStore::Iterator::get(Udjat::Response::Table &value) const {

		DataStore::Iterator it{*this};
		if(!it) {
			return false;
		}

		value.last_modified(file->get<Header>(0).last_modified);

		// Start report
		std::vector<std::string> column_names;

		column_names.push_back("_row");
		for(auto col : cols) {
			column_names.push_back(col->name());
		}

		value.start(column_names);

		size_t items = 0;
		while(it) {

			for(auto col : column_names) {

#ifdef DEBUG
				if(!strcasecmp(col.c_str(),"_row")) {
					value.push_back(it.row);
					continue;
				}
#endif // DEBUG

				value.push_back(it[col.c_str()]);
			}

			items++;
			it++;
		}
		value.count(items);

		return true;


	}

 }

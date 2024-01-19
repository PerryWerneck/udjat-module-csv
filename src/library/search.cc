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
  * @brief Get lists from container.
  */
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/datastore/container.h>
 #include <udjat/tools/logger.h>

 using namespace std;

 namespace Udjat {

	DataStore::Iterator DataStore::Container::IteratorFactory(const char *path) const {
		return Iterator::Factory(active_file,columns(),path);
	}

	bool DataStore::Container::head(const char *path, Udjat::Abstract::Response &response) const {

		DataStore::Iterator it{IteratorFactory(path)};
		if(!it) {
			return false;
		}

		if(expires) {
			response.expires(time(0)+expires);
		}

		response.last_modified(last_modified());
		response.count(it.count());

		return true;
	}

	bool DataStore::Container::get(const char *path, Udjat::Response::Table &value) const {

		debug(__FUNCTION__,"('",path,"')");

		DataStore::Iterator it{IteratorFactory(path)};
		if(!it) {
			return false;
		}

		if(expires) {
			value.expires(time(0)+expires);
		}

		value.last_modified(last_modified());

		// Start report
		std::vector<std::string> column_names;

		for(auto col : cols) {
			column_names.push_back(col->name());
		}

		value.start(column_names);

		size_t items = 0;
		while(it) {

			for(auto col : column_names) {
				value.push_back(it[col.c_str()]);
			}

			items++;
			it++;
		}
		value.count(items);

		return true;

	}

	bool DataStore::Container::get(const char *path, Udjat::Value &value) const {

		// It's asking for a single row??
		if(strncasecmp(path,"row/",4) == 0) {

			DataStore::Iterator it{active_file,columns()};
			it = atoi(path+4);
			if(it) {
				it.get(value);
				return true;
			}

			return false;
		}

		DataStore::Iterator it{IteratorFactory(path)};
		if(!it) {
			return false;
		}

		it.get(value);
		return true;

	}

	bool DataStore::Container::get(const char *path, Udjat::Response::Value &value) const {

		DataStore::Iterator it{IteratorFactory(path)};

		if(it) {

			if(expires) {
				value.expires(time(0)+expires);
			}

			value.last_modified(last_modified());
			value.count(it.count());
			it.get(value);
			return true;
		}

		return false;

	}

 }


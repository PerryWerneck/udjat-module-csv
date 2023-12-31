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
  * @brief Brief description of this source.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/datastore/container.h>
 #include <udjat/tools/datastore/loader.h>
 #include <udjat/tools/datastore/file.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/timestamp.h>
 #include <private/structs.h>

 using namespace std;

 namespace Udjat {

	DataStore::Container::Container(const XML::Node &definition)
		: name{Quark{definition,"name","",false}.c_str()},
			path{Object::getAttribute(definition,"path","")},
			filespec{Object::getAttribute(definition,"filespec",".*")} {

		if(!*name) {
			throw runtime_error("Required attribute 'name' is missing");
		}

		if(!*path) {
			throw runtime_error("Required attribute 'path' is missing");
		}

		for(XML::Node child = definition.child("column"); child; child = child.next_sibling("column")) {

			const char *type = child.attribute("type").as_string("string");

			std::shared_ptr<Abstract::Column> col;

			if(!strcasecmp(type,"int")) {
				col = make_shared<Column<int>>(child);
			} else if(!strcasecmp(type,"uint")) {
				col = make_shared<Column<unsigned int>>(child);
			} else if(!strcasecmp(type,"string")) {
				col = make_shared<Column<std::string>>(child);
			} else {
				throw runtime_error(Logger::String{"Unexpected column type: ",type});
			}

			cols.push_back(col);

			for(auto alias : String{child,"aliases","",false}.split(",")) {
				alias.strip();
				if(!alias.empty()) {
					aliases.emplace_back(alias.as_quark(),col);
				}
			}

		}

	}

	DataStore::Container::~Container() {
	}

	void DataStore::Container::state(const State) {
	}

	size_t DataStore::Container::size() const {
		return active_file->get<size_t>(active_file->get<Header>(0).primary_offset);
	}

	TimeStamp DataStore::Container::update_time() const {
		return TimeStamp{active_file->get<Header>(0).updated};
	}

	void DataStore::Container::load() {
		auto file = Loader::CSV{*this,path,filespec}.load();
		file->map(); // Map file in memory.
		active_file = file;
		Logger::String{"New storage with ",size()," record(s) is active"}.trace(name);
		state(size() ? Ready : Empty);
	}

	const DataStore::Container::Iterator DataStore::Container::find(const char *key) const {
		return begin().search(key);
	}

	const DataStore::Container::Iterator DataStore::Container::find(const char *column_name, const char *key) const {

		auto col = this->column(column_name);

		if(!col) {
			throw std::system_error(ENOENT,std::system_category(),column_name);
		}

		if(!col->indexed()) {
			throw runtime_error(Logger::Message{"Column '{}' is not indexed",column_name});
		}

		throw runtime_error("Incomplete");

	}

	bool DataStore::Container::get(const char *path, Udjat::Value &value) const {

		while(*path && *path == '/') {
			path++;
		}

		const char *mark = strchr(path,'/');

		if(!mark) {

			// Primary key search.
			Iterator it{find(path)};

			if(it) {
				debug("Found resource '",it.to_string(),"'");
				it.get(value);
				return true;
			}

			return false;

		}

		mark++;

		if(strncasecmp(path,"rownumber/",10) == 0) {

			Iterator it{begin()};

			it.set(atoi(path+10)-1);

			if(it) {
				it.get(value);
				return true;
			}

		}


		/*
		if(!strncasecmp(path,"id/",3)) {

			// It's an id
			get((size_t) atoi(path+3)-1, value);
			return true;

		} else if(!strchr(path,'/')) {

			debug("Searching for primary key '",path,"'");
			Resource res{find(path)};

			if(res) {
				debug("Found resource '",res.to_string(),"'");
				res.get(value);
				return true;
			}

		} else {

			// TODO: Search in the format [Column-name]/[Column-value]


			return false;

		}
		*/

		return false;

	}

	std::shared_ptr<DataStore::Abstract::Column> DataStore::Container::column(const char *name) const {

		for(auto col : cols) {
			if(!strcasecmp(name,col->name())) {
				return col;
			}
		}

		// Check for aliases.
		for(const Alias &alias : aliases) {
			if(!strcasecmp(name,alias.name)) {
				return alias.col;
			}
		}

		return std::shared_ptr<Abstract::Column>();
	}

	size_t DataStore::Container::column_index(const char *name) const {

		size_t index = 0;
		for(auto col : cols) {
			if(!strcasecmp(name,col->name())) {
				return index;
			}
			index++;
		}

		// Check for aliases.
		for(const Alias &alias : aliases) {
			if(!strcasecmp(name,alias.name)) {
				size_t index = 0;
				for(auto col : cols) {
					if(col.get() == alias.col.get()) {
						return index;
					}
					index++;
				}
			}
		}

		return ((size_t) -1);

	}
 }


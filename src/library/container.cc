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
 #include <udjat/tools/datastore/columns/ipv4.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/timestamp.h>
 #include <udjat/tools/singleton.h>
 #include <private/structs.h>
 #include <udjat/tools/quark.h>

 using namespace std;

 namespace Udjat {

	class Controller : public Singleton::Container<DataStore::Container> {
	public:
		static Controller & getInstance() {
			static Controller instance;
			return instance;
		}

	} containers;

	DataStore::Container::Container(const XML::Node &definition)
		: name{Quark{definition,"name"}.c_str()},
			path{Object::getAttribute(definition,"sources-from","")},
			filespec{Object::getAttribute(definition,"sources-file-filter",".*")} {

		if(!*name) {
			throw runtime_error("Required attribute 'name' is missing");
		}

		if(!*path) {
			throw runtime_error("Required attribute 'path' is missing");
		}

		size_t index = 0;
		for(XML::Node child = definition.child("column"); child; child = child.next_sibling("column")) {

			const char *type = child.attribute("type").as_string("string");

			std::shared_ptr<Abstract::Column> col;

			if(!strcasecmp(type,"int")) {
				col = make_shared<Column<int>>(child,index++);

			} else if(!strcasecmp(type,"uint")) {
				col = make_shared<Column<unsigned int>>(child,index++);

			} else if(!strcasecmp(type,"string")) {
				col = make_shared<Column<std::string>>(child,index++);

			} else if(!strcasecmp(type,"ipv4")) {
				col = make_shared<Column<in_addr>>(child,index++);

			} else if(!strncasecmp(type,"bool",4)) {
				col = make_shared<Column<bool>>(child,index++);

			} else {
				throw runtime_error(Logger::String{"Unexpected column type: ",type});
			}

			cols.push_back(col);

			for(auto alias : String{child,"aliases",""}.split(",")) {
				alias.strip();
				if(!alias.empty()) {
					aliases.emplace_back(alias.as_quark(),col);
				}
			}

		}

		containers.getInstance().push_back(this);
	}

	DataStore::Container::~Container() {
		containers.getInstance().remove(this);
	}

	void DataStore::Container::state(const State) {
	}

	DataStore::Container * DataStore::Container::find(const Request &request) {

		const char *path = request.path();
		while(*path && *path == '/') {
			path++;
		}

		const char *ptr = strchr(path,'/');
		if(ptr) {
			return containers.getInstance().find(string{path,(size_t)(ptr-path)}.c_str());
		}
		return containers.getInstance().find(path);
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
		Logger::String{"New storage with ",size()," record(s) is active (",TimeStamp{last_modified()}.to_string(),")"}.trace(name);
		state(size() ? Ready : Empty);
	}

	const DataStore::Container::Iterator DataStore::Container::find(const char *key) const {
		return begin().find(key);
	}

	const DataStore::Container::Iterator DataStore::Container::find(const char *column, const char *key) const {
		return begin(column).find(key);
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

	time_t DataStore::Container::last_modified() const {

		time_t timestamp = 0;

		if(active_file && active_file->mapped()) {
			const char *filename = active_file->get_ptr<char>(sizeof(Header));
			while(*filename) {
				time_t *tm = (time_t *) (filename + strlen(filename)+1);
				debug("Filename: '",filename,"' timestamp: ",TimeStamp(*tm).to_string());
				if(!timestamp || *tm < timestamp) {
					timestamp = *tm;
				}
				filename = ((const char *) (tm+1));
			}
		}

		return timestamp;

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


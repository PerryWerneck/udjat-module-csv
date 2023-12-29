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

			if(!strcasecmp(type,"int")) {
				cols.emplace_back(make_shared<Column<int>>(child));
			} else if(!strcasecmp(type,"uint")) {
				cols.emplace_back(make_shared<Column<unsigned int>>(child));
			} else if(!strcasecmp(type,"string")) {
				cols.emplace_back(make_shared<Column<std::string>>(child));
			} else {
				throw runtime_error(Logger::String{"Unexpected column type: ",type});
			}

		}

	}

	DataStore::Container::~Container() {
	}

	void DataStore::Container::state(const State) {
	}

	void DataStore::Container::load() {
		auto file = Loader::CSV{*this,path,filespec}.load();
		file->map();

		size_t records = *file->get<size_t>(file->get<Header>(0)->primary_offset);

		debug("Got ",records," records");
		state(records ? Ready : Empty);

	}

	DataStore::Abstract::Column::Column(const XML::Node &node)
		: cname{Quark{node,"name","unnamed",false}.c_str()} {

		if(node.attribute("primary-key").as_bool(false) || node.attribute("primary").as_bool(false)) {
			type = Primary;
		} else if(node.attribute("index").as_bool(false)) {
			type = Index;
		} else {
			type = Value;
		}

		format.length = (uint8_t) node.attribute("length").as_uint(format.length);

		if(node.attribute("zero-fill").as_bool(false)) {
			format.leftchar = '0';
		}
	}

 }


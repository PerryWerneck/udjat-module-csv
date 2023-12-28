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
 #include <udjat/tools/threadpool.h>
 #include <udjat/tools/quark.h>
 #include <udjat/tools/memdb/simpletable.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>
 #include <udjat/tools/object.h>
 #include <udjat/tools/file.h>
 #include <regex>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <string>

 using namespace std;

 namespace Udjat {

	MemCachedDB::Table::Table(const XML::Node &definition)
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
				columns.emplace_back(make_shared<Column<int>>(child));
			} else if(!strcasecmp(type,"uint")) {
				columns.emplace_back(make_shared<Column<unsigned int>>(child));
			} else if(!strcasecmp(type,"string")) {
				columns.emplace_back(make_shared<Column<std::string>>(child));
			} else {
				throw runtime_error(Logger::String{"Unexpected column type: ",type});
			}

		}

	}

	MemCachedDB::Abstract::Column::Column(const XML::Node &node)
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

	MemCachedDB::Table::~Table() {
	}

	const std::string & MemCachedDB::Abstract::Column::apply_layout(std::string &str) const {

		if(str.size() == format.length) {
			return str;
		}

		if(str.size() < format.length) {
			std::string rc;
			rc.resize((format.length - str.size()),format.leftchar);
			rc.append(str);
			str = rc;
		}

		return str;
	}

	std::string MemCachedDB::Abstract::Column::to_string(const void *datablock) const {
		String str{convert(datablock)};
		if(format.length) {
			apply_layout(str);
		}
		return str;
	}

 }


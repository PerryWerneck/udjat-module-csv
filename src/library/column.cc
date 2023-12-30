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
  * @brief Implement datastore columns.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/converters.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/logger.h>

 namespace Udjat {

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

	const std::string & DataStore::Abstract::Column::apply_layout(std::string &str) const {

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

	std::string DataStore::Abstract::Column::to_string(const void *datablock) const {
		String str{convert(datablock)};
		if(format.length) {
			apply_layout(str);
		}
		return str;
	}

	std::string DataStore::Abstract::Column::to_string(std::shared_ptr<File> file, size_t offset) {

		if(!offset) {
			return "";
		}

		if(length()) {
			return to_string(file->get_void_ptr(offset));
		}

		return file->get_ptr<char>(offset);
	}

	bool DataStore::Abstract::Column::comp(std::shared_ptr<File> file, size_t loffset, size_t roffset) {

		size_t len = length();
		if(len) {

			// It's a data block
			char ldata[len];
			file->read(loffset,ldata,len);

			char rdata[len];
			file->read(roffset,rdata,len);

			return comp(ldata,rdata);

		}

		// It's an string
		return comp(file->read(loffset).c_str(),file->read(roffset).c_str());

	}

 }


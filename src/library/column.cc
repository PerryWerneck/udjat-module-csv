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
 #include <udjat/tools/quark.h>

 namespace Udjat {

	DataStore::Abstract::Column::Column(const XML::Node &node, size_t i)
		: cname{Quark{node,"name","unnamed"}.c_str()},index{i} {

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

	int DataStore::Abstract::Column::comp(std::shared_ptr<File> file, const size_t *row, const char *key) const {
		return strncasecmp(to_string(file,row).c_str(),key,strlen(key));
	}

	std::string DataStore::Abstract::Column::to_string(std::shared_ptr<File> file, size_t offset) const {

		if(!offset) {
			return "";
		}

		if(length()) {
			return to_string(file->get_void_ptr(offset));
		}

		return file->get_ptr<char>(offset);
	}

	bool DataStore::Abstract::Column::less(std::shared_ptr<File> file, const size_t *lrow, const size_t *rrow) const {

		size_t len = length();
		if(len) {

			// It's a data block
			char ldata[len];
			file->read(lrow[index],ldata,len);

			char rdata[len];
			file->read(rrow[index],rdata,len);

			return less(ldata,rdata);

		}

		// It's an string
		return less(file->read(lrow[index]).c_str(),file->read(rrow[index]).c_str());

	}

	/*
	std::string DataStore::Abstract::Column::to_string(const void *datablock) const {
		String str{to_string(datablock)};
		if(format.length) {
			apply_layout(str);
		}
		return str;
	}
	*/


	std::string DataStore::Abstract::Column::to_string(std::shared_ptr<File> file, const size_t *row) const {
		std::string str{to_string(file,row[index])};
		if(format.length) {
			apply_layout(str);
		}
		return str;
	}

 }


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
 #include <udjat/tools/value.h>
 #include <private/value.h>
 #include <string>

 using namespace std;

 namespace Udjat {

	bool DataStore::Value::isNull() const {
		return offset == 0;
	}

	bool DataStore::Value::isNumber() const {

		if(col->length() == 0 || col->formatted()) {
			return false;
		}

		if(!offset) {
			return false;
		}

		string str = col->to_string(file,offset);
		for(const char *ptr = str.c_str();*ptr;ptr++) {
			if(*ptr < '0' || *ptr > '9') {
				return false;
			}
		}

		return true;
	}

	const Udjat::Value & DataStore::Value::get(std::string &value) const {
		if(offset) {
			value = col->to_string(file,offset);
		} else {
			value.clear();
		}
		return *this;
	}

 }

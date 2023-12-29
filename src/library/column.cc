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

 namespace Udjat {

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

 }


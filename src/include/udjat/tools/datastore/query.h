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
  * @brief Declares CSVLoader query.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/datastore/file.h>
 #include <udjat/tools/quark.h>
 #include <memory>
 #include <vector>

 namespace Udjat {

 	namespace DataStore {

		class UDJAT_API Query {
		private:
			const char *name;

		protected:

			Query(const XML::Node &node) : name{Quark{node,"name"}.c_str()} {
			}

		public:

			virtual ~Query();

			static std::shared_ptr<Query> Factory(const XML::Node &node, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols);

			inline bool operator==(const char *n) const noexcept {
				return strcasecmp(name,n) == 0;
			}

			inline const char * c_str() const noexcept {
				return name;
			}

			virtual DataStore::Iterator call(const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols,std::shared_ptr<File>, const Request &request) const = 0;

		};

 	}

 }

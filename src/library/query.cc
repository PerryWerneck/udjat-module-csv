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
  * @brief Implement query.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/query.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	static uint16_t get_column_by_name(const std::vector<std::shared_ptr<DataStore::Abstract::Column>> cols, const char *name) {

		for(size_t ix = 0; ix < cols.size(); ix++) {

			if(!strcasecmp(cols[ix]->name(),name)) {
				return (uint16_t) ix;
			}

		}

		throw runtime_error(Logger::String{"Required column '",name,"' was not found"});

	}

	std::shared_ptr<DataStore::Query> DataStore::Query::Factory(const XML::Node &node, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols) {

		/// @brief IPV4 Network query.
		class QueryNetV4 : public DataStore::Query {
		private:

			struct Column {
				uint16_t ip;		///< @brief ID of the Reference IP column.
				uint16_t mask;		///< @brief ID of the netmask column.
			} column;

		public:
			QueryNetV4(const XML::Node &node, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> cols) : DataStore::Query{node} {

				column.ip = get_column_by_name(cols,node.attribute("network-from").as_string("undefined"));
				column.mask = get_column_by_name(cols,node.attribute("mask-from").as_string("undefined"));

			}

		};

		const char *type = node.attribute("type").as_string("undefined");
		if(!strcasecmp(type,"netv4")) {
			return make_shared<QueryNetV4>(node,cols);
		}

		throw runtime_error(Logger::String{"Unknown or invalid query '",type,"'"});
	}


	DataStore::Query::~Query() {
	}

 }

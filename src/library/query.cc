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
 #include <udjat/tools/datastore/columns/ipv4.h>
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
				uint16_t index;		///< @brief Index column.
				uint16_t ip;		///< @brief ID of the Reference IP column.
				uint16_t mask;		///< @brief ID of the netmask column.
			} column;

		public:
			QueryNetV4(const XML::Node &node, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> cols) : DataStore::Query{node} {

				column.ip = get_column_by_name(cols,node.attribute("network-from").as_string("undefined"));
				if(!dynamic_cast<DataStore::Column<in_addr> *>(cols[column.ip].get())) {
					throw runtime_error("Invalid column type");
				}

				column.mask = get_column_by_name(cols,node.attribute("mask-from").as_string("undefined"));
				if(!dynamic_cast<DataStore::Column<in_addr> *>(cols[column.mask].get())) {
					throw runtime_error("Invalid column type");
				}

				column.index = get_column_by_name(cols,node.attribute("index").as_string("undefined"));
				if(!dynamic_cast<DataStore::Column<in_addr> *>(cols[column.mask].get())) {
					throw runtime_error("Invalid column type");
				}

				if(!cols[column.index]->indexed()) {
					throw runtime_error("Invalid index column");
				}

			}

			DataStore::Iterator call(const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols,std::shared_ptr<File>,const Request &request) const override {

				size_t key = 0;
				const char *path = request.path();

				if(path && *path) {

					// Use IP from path
					debug("Selecting network from '",path,"'");

					struct in_addr addr;

					if(!inet_pton(AF_INET, path, &addr)) {
						throw std::system_error(errno,std::system_category(),path);
					}

					key = (size_t) htonl(addr.s_addr);

				} else {

					// Use request's ip address.

					throw runtime_error("Incomplete - Cant search from request's origin address");

				}

				const auto *ip = dynamic_cast<DataStore::Column<in_addr> *>(cols[column.ip].get());
				const auto *mask = dynamic_cast<DataStore::Column<in_addr> *>(cols[column.mask].get());


				throw runtime_error("Pending implementation");

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

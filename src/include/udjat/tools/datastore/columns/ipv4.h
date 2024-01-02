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
  * @brief Declare IPV4 column.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/logger.h>
 #include <udjat/net/ip/address.h>

 #include <stdexcept>

 #include <netinet/in.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>

 namespace Udjat {

	namespace DataStore {

		template <>
		class UDJAT_API Column<in_addr> : public Abstract::Column {
		public:
			Column(const XML::Node &node) : Abstract::Column{node} {
			}

			size_t length() const noexcept override {
				return sizeof(in_addr);
			};

			size_t store(Deduplicator &store, const char *text) const override {

				// FIXME: Is the deduplication really usefull here?

				in_addr addr;
				if(!inet_aton(text, &addr)) {
					throw std::runtime_error(Logger::String{"Invalid IPV4 '",text,"'"});
				}
				return store.insert(&addr,sizeof(addr));
			}

			bool comp(const void *lhs, const void *rhs) const override {
				return htonl(((const in_addr *) lhs)->s_addr) < htonl(((const in_addr *) rhs)->s_addr);
			}

			std::string convert(const void *datablock) const override {
				return std::to_string(*((in_addr *) datablock));
			}

		};
	}

 }


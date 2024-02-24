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
  * @brief Implements ipv4 column.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/datastore/columns/ipv4.h>
 #include <udjat/net/ip/address.h>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	size_t DataStore::Column<in_addr>::save(Deduplicator &, const char *text) const {

		sockaddr_storage addr = IP::Factory(text);
		if(addr.ss_family != AF_INET) {
			throw runtime_error(Logger::String{"Cant convert address ",text," to an IPV4 value"});
		}
		return  (size_t) htonl(((sockaddr_in *) &addr)->sin_addr.s_addr);

	}

	int DataStore::Column<in_addr>::comp(std::shared_ptr<File>, const size_t *row, const char *key) const {

		sockaddr_storage addr = IP::Factory(key);
		if(addr.ss_family != AF_INET) {
				throw runtime_error(Logger::String{"Cant convert address ",text," to an IPV4 value"});
		}

		return row[index] - htonl(((sockaddr_in *) &addr)->sin_addr.s_addr);
	}

	bool DataStore::Column<in_addr>::less(std::shared_ptr<File>, const size_t *lrow, const size_t *rrow) const {
		return lrow[index] < rrow[index];
	}

	std::string DataStore::Column<in_addr>::to_string(std::shared_ptr<File>, const size_t *row) const {

		in_addr addr;
		memset(&addr,0,sizeof(addr));
		addr.s_addr = htonl(row[index]);

		return std::to_string(addr);
	}

 }

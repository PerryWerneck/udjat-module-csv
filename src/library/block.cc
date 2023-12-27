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
 #include <udjat/tools/memdb/file.h>
 #include <udjat/tools/memdb/datastore.h>
 #include <cstring>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	 DataStore::Block::Block(std::shared_ptr<MemCachedDB::File> f, const void *data, size_t l) : file{f}, length{l} {

		// computes the hash of a data using a variant
		// of the Fowler-Noll-Vo hash function
		{
			constexpr std::uint64_t prime{0x100000001B3};
			std::uint64_t result{0xcbf29ce484222325};

			for (size_t i{}; i < length; i++) {
				result = (result * prime) ^ ((uint8_t *) data)[i];
			}
			this->hash = (size_t) result;
		}

	 }

	 bool DataStore::Block::compare(const void *src) const {

		if(!offset) {
			throw logic_error("This block has no data");
		}

		uint8_t data[length];
		file->read(offset,data,length);

		return memcmp(src,data,length) == 0;

	 }

	 bool DataStore::Block::operator==(const Block &b) const {

		if(b.length != length || b.hash != hash) {
			return false;
		}

		if(!offset) {
			throw logic_error("This block has no data");
		}

		uint8_t data[length];
		file->read(offset,data,length);

		return b.compare(data);
	 }

 }

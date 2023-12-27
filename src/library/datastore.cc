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
 #include <private/datastore.h>
 #include <stdexcept>

 using namespace std;

 size_t DataStore::insert(const void *data, size_t length) {

	class InnerStore : public Block {
	private:
		const void *ptr;

	public:
		InnerStore(std::shared_ptr<MemCachedDB::File> file, const void *data, size_t length) : Block{file,data,length}, ptr{data} {
		}

		bool compare(const void *src) const {
			return memcmp(ptr,src,length) == 0;
		}

		bool operator==(const Block &b) const override {

			if(b.length != length || b.hash != hash) {
				return false;
			}

			return b.compare(ptr);

		 }

	};

	InnerStore record{file,data,length};


	throw runtime_error("Incomplete");
 }

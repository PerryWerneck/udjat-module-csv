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
 #include <string>

 using namespace std;

 namespace Udjat {

	// int32_t

	size_t DataStore::Column<int32_t>::save(Deduplicator &, const char *text) const {
		return (size_t) stoi(text);
	}

	int DataStore::Column<int32_t>::comp(std::shared_ptr<File>, const size_t *row, const char *key) const {
		return row[index] - ((size_t) stoi(key));
	}

	bool DataStore::Column<int32_t>::less(std::shared_ptr<File>, const size_t *lrow, const size_t *rrow) const {
		return lrow[index] < rrow[index];
	}

	std::string DataStore::Column<int32_t>::to_string(std::shared_ptr<File>, const size_t *row) const {
		return std::to_string((uint32_t) row[index]);
	}

	// uint32_t

	size_t DataStore::Column<uint32_t>::save(Deduplicator &, const char *text) const {
		return (size_t) stoul(text);
	}

	int DataStore::Column<uint32_t>::comp(std::shared_ptr<File>, const size_t *row, const char *key) const {
		return row[index] - ((size_t) stoul(key));
	}

	bool DataStore::Column<uint32_t>::less(std::shared_ptr<File>, const size_t *lrow, const size_t *rrow) const {
		return lrow[index] < rrow[index];
	}

	std::string DataStore::Column<uint32_t>::to_string(std::shared_ptr<File>, const size_t *row) const {
		return std::to_string((uint32_t) row[index]);
	}

 }

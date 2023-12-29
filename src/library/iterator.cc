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
  * @brief Implements container iterator.
  */

 #include <config.h>
 #include <udjat/tools/datastore/container.h>

 using namespace std;

 namespace Udjat {

	DataStore::Container::Iterator DataStore::Container::begin() const {
		return Iterator(active_file,cols,0);
	}

	DataStore::Container::Iterator DataStore::Container::end() const {
		return Iterator(active_file,cols,size());
	}

	DataStore::Container::Iterator::Iterator(std::shared_ptr<File> file, const std::vector<std::shared_ptr<Abstract::Column>> &cols, size_t id)
		: Resource(file,cols,id) {
	}

	DataStore::Container::Iterator::~Iterator() {
	}

 }

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
  * @brief Declare DataStore value.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/datastore/file.h>

 namespace Udjat {

	namespace DataStore {

		class UDJAT_API Value : public Udjat::Value {
		private:
			std::shared_ptr<File> file;
			std::shared_ptr<Abstract::Column> col;
			const size_t *rowptr;

		public:
			Value(std::shared_ptr<File> f, std::shared_ptr<Abstract::Column> c, const size_t *r)
				: file{f}, col{c}, rowptr{r} {
			}

			bool isNumber() const override;
			const Udjat::Value & get(std::string &value) const override;

		};

	}

 }

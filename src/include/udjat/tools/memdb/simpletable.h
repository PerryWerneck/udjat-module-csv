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
  * @brief Declare a simple dynamic created and memory mapped table.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/object.h>
 #include <udjat/tools/xml.h>
 #include <memory>
 #include <udjat/tools/memdb/file.h>

 namespace Udjat {

	namespace MemCachedDB {

		class UDJAT_API Table {
		public:

			enum State : int {
				Undefined,
				Loading,
				Failed,
				Loaded,
				Empty
			};

		private:

			class Loader;
			friend class Loader;

			/// @brief The table name.
			const char *name;

			/// @brief The table path.
			const char *path;

			/// @brief Regex expression to filter path contents.
			const char *filespec;

			/// @brief The table file.
			std::shared_ptr<MemCachedDB::File> file;

		protected:

			virtual void state(const State state) noexcept = 0;
			virtual State state() const noexcept = 0;

			template <typename T>
			class UDJAT_API Column {
			protected:
				const char *name;
				T data;

			public:
				Column(const char *n) : name{n} {
				}

				inline std::string to_string() const noexcept {
					return std::to_string(data);
				}


			 };

		public:

			/// @brief Build table from XML definition.
			Table(const XML::Node &definition);
			virtual ~Table();

			void load();

		};

	}


 }

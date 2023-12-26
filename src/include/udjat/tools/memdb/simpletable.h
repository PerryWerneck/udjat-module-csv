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
 #include <udjat/tools/quark.h>
 #include <memory>
 #include <udjat/tools/memdb/file.h>
 #include <vector>
 #include <string>

 namespace Udjat {

	namespace MemCachedDB {

		namespace Abstract {

			class UDJAT_API Column {
			protected:
				const char *cname;

			public:
				Column(const XML::Node &node) : cname{Quark{node,"name","unnamed",false}.c_str()} {
				}

				inline const char * name() const noexcept {
					return cname;
				}

				virtual std::string to_string() const noexcept = 0;
			};

		}

		template <typename T>
		class UDJAT_API Column : public Abstract::Column {
		protected:
			T data;

		public:
			Column(const XML::Node &node) : Abstract::Column{node} {
			}

			std::string to_string() const noexcept override {
				return std::to_string(data);
			}

		};

		template <>
		class UDJAT_API Column<std::string> : public Abstract::Column, public std::string {
		public:
			Column(const XML::Node &node) : Abstract::Column{node} {
			}

			std::string to_string() const noexcept override {
				return std::string{*this};
			}

		};

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

			std::vector<std::shared_ptr<Abstract::Column>> columns;

		public:

			/// @brief Build table from XML definition.
			Table(const XML::Node &definition);
			virtual ~Table();

			void load();

		};

	}


 }

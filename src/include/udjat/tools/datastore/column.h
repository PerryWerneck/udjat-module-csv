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
  * @brief Declare data store columns.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/converters.h>

 namespace Udjat {

	namespace DataStore {

		namespace Abstract {

			/// @brief Descritor de coluna.
			class UDJAT_API Column {
			public:
				enum Type {
					Value,		///< @brief It's a simple field.
					Primary,	///< @brief It's a primary key.
					Index,		///< @brief It's an index.
				};

			private:

				Type type = Value;

				struct {
					uint8_t length = 0;		///< @brief Length of the output string.
					char	leftchar = ' ';	///< @brief Char to fill.
				} format;

			protected:
				const char *cname;

			public:
				Column(const XML::Node &node);

				bool operator==(const char *n) const {
					return strcasecmp(cname,n) == 0;
				}

				inline const char * name() const noexcept {
					return cname;
				}

				/// @brief Is this column part of the primary index?
				inline bool key() const noexcept {
					return type == Primary;
				}

				/// @brief Get the size of data-block for this column.
				/// @retval 0 The data-block length has variable size.
				virtual size_t length() const noexcept = 0;

				/// @brief Convert data from string to object format and store it.
				/// @return Offset of the stored data.
				virtual size_t store(DataStore &store, const char *text) const = 0;

				/// @brief Compare two values.
				/// @see std::less
				/// @return True if *lhs < *rhs
				virtual bool comp(const void *lhs, const void *rhs) const = 0;

				/// @brief Convert datablock to string.
				virtual std::string convert(const void *datablock) const = 0;

				/// @brief Format string.
				/// @param str String to format.
				/// @return str
				const std::string & apply_layout(std::string &str) const;

				std::string to_string(const void *datablock) const;

			};

		}

		template <typename T>
		class UDJAT_API Column : public Abstract::Column {
		public:
			Column(const XML::Node &node) : Abstract::Column{node} {
			}

			size_t length() const noexcept override {
				return sizeof(T);
			};

			size_t store(Udjat::DataStore &store, const char *text) const override {
				T value{Udjat::from_string<T>(text)};
				return store.insert(&value,sizeof(value));
			}

			bool comp(const void *lhs, const void *rhs) const override {
				return *((T *) lhs) < *((T *) rhs);
			}

			std::string convert(const void *datablock) const override {
				return std::to_string(*((T *) datablock));
			}

		};

		template <>
		class UDJAT_API Column<std::string> : public Abstract::Column {
		public:
			Column(const XML::Node &node) : Abstract::Column{node} {
			}

			size_t length() const noexcept override {
				return 0;
			};

			size_t store(Udjat::DataStore &store, const char *text) const override {
				return store.insert(text,strlen(text)+1);
			}

			bool comp(const void *lhs, const void *rhs) const override {
				return strcasecmp((const char *) lhs, (const char *) rhs) < 0;
			}

			std::string convert(const void *datablock) const override {
				return std::string{datablock ? (const char *) datablock : ""};
			}

		};

	}

 }

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
 #include <udjat/tools/datastore/deduplicator.h>

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
				size_t index;

				virtual std::string to_string(std::shared_ptr<File> file, size_t offset) const;

				/// @brief Compare two values.
				/// @see std::less
				/// @return True if *lhs < *rhs
				virtual bool less(const void *lhs, const void *rhs) const;

				/// @brief Convert datablock to string.
				virtual std::string to_string(const void *datablock) const;

			public:
				Column(const XML::Node &node,size_t index);

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

				/// @brief Is this column indexed?
				inline bool indexed() const noexcept {
					return type == Index;
				}

				/// @brief Is this column formatted?
				inline bool formatted() const noexcept {
					return format.length != 0 && format.leftchar != 0;
				}

				/// @brief Get column offset.
				inline size_t offset(const size_t *rowptr) const noexcept {
					return rowptr[index];
				}

				/// @brief Get the size of data-block for this column.
				/// @retval 0 The data-block is a string.
				virtual size_t length() const noexcept = 0;

				/// @brief Convert data from string to object format and store it.
				/// @param destination The deduplicator used to store the data.
				/// @param text The string to store.
				/// @return Offset of the stored data.
				virtual size_t save(Deduplicator &destination, const char *text) const = 0;

				/// @brief Load and compare two values, used while loading.
				/// @param file The file being loaded.
				/// @return True if loffset < roffset.
				virtual bool less(std::shared_ptr<File> file, const size_t *lrow, const size_t *rrow) const;

				/// @brief Compare column with string.
				/// @return Result of test (strcasecmp)
				virtual int comp(std::shared_ptr<File> file, const size_t *row, const char *key) const;

				/// @brief Format string.
				/// @param str String to format.
				/// @return str
				const std::string & apply_layout(std::string &str) const;

				virtual std::string to_string(std::shared_ptr<File> file, const size_t *row) const;

			};

		}

		template <typename T>
		class UDJAT_API Column : public Abstract::Column {
		protected:

			bool less(const void *lhs, const void *rhs) const override {
				return *((T *) lhs) < *((T *) rhs);
			}

			std::string to_string(const void *datablock) const override {
				return std::to_string(*((T *) datablock));
			}

			virtual size_t write(Deduplicator &store, const T &value) const {
				return store.insert(&value,sizeof(value));
			}

		public:
			Column(const XML::Node &node, size_t index) : Abstract::Column{node,index} {
			}

			size_t length() const noexcept override {
				return sizeof(T);
			};

			size_t save(Deduplicator &store, const char *text) const override {
				return write(store,Udjat::from_string<T>(text));
			}

		};

		template <>
		class UDJAT_API Column<std::string> : public Abstract::Column {
		protected:
			bool less(const void *lhs, const void *rhs) const override {
				return strcasecmp((const char *) lhs, (const char *) rhs) < 0;
			}

			std::string to_string(const void *datablock) const override {
				return std::string{datablock ? (const char *) datablock : ""};
			}

		public:
			Column(const XML::Node &node,size_t index) : Abstract::Column{node,index} {
			}

			size_t length() const noexcept override {
				return 0;
			};

			size_t save(Deduplicator &store, const char *text) const override {
				return store.insert(text,strlen(text)+1);
			}

		};

	}

 }

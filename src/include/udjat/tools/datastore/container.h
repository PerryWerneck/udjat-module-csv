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
  * @brief Declare data store container.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/timestamp.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/converters.h>
 #include <udjat/tools/response.h>
 #include <udjat/tools/report.h>
 #include <vector>
 #include <iterator>

 namespace Udjat {

 	namespace DataStore {

		enum State : uint8_t {
			Undefined,		///< @brief Data store is in undefined state.
			Updating,		///< @brief Updating from data source.
			Failed,			///< @brief Update failed.
			Ready,			///< @brief Data was loaded from source.
			Empty			///< @brief Empty data.
		};

		UDJAT_API DataStore::State StateFactory(const char *str);

		/// @brief A data store container.
		class UDJAT_API Container {
		private:

			const char *name;
			const char *path;
			const char *filespec;

			/// @brief The current file holding the real data.
			std::shared_ptr<File> active_file;

			/// @brief The data columns.
			std::vector<std::shared_ptr<Abstract::Column>> cols;

			struct Alias {
				const char *name;
				std::shared_ptr<Abstract::Column> col;

				Alias(const char *n, std::shared_ptr<Abstract::Column> c) : name{n}, col{c} {
				}

			};

			std::vector<Alias> aliases;

		public:

			/// @brief A resource inside the container.
			class Iterator {
			private:
				friend class Container;
				Iterator(const Container &container);

				std::shared_ptr<File> file;										///< @brief The data file
				const std::vector<std::shared_ptr<Abstract::Column>> &cols;		///< @brief The column definitions.

				struct {
					std::string key;											///< @brief Search key.
					uint16_t column = (uint16_t) -1;							///< @brief Search column (-1 when using primary index).
				} filter;
				size_t row = 0;													///< @brief Selected row.
				const size_t *ixptr = nullptr;									///< @brief Pointer to index data.

				/// @brief Get pointer to row descriptor
				const size_t * rowptr() const;

			public:
				using iterator_category = std::random_access_iterator_tag;
				using difference_type   = int;
				using value_type        = Iterator;
				using pointer           = Iterator *;  // or also value_type*
				using reference         = Iterator &;  // or also value_type&

				// constexpr reference operator*() const noexcept { return *this; }
				// constexpr pointer operator->() const noexcept { return this; }

				Iterator(Iterator &src, size_t id);
				~Iterator();

				size_t count() const;

				Iterator& set(size_t id = 0);

				inline Iterator & operator=(size_t id) {
					return set(id);
				}

				operator bool() const;

				virtual std::string to_string() const;
				std::string operator[](const char *column) const;

				Udjat::Value & get(Udjat::Value &value) const;

				// Search
				/// @brief Is the iterator 'similar' to key?
				inline bool operator== (const char *key) const {
					return compare(key) == 0;
				}

				/// @brief Compare resource with 'key'
				/// @param key the string to compare.
				/// @return result of the casecmp test.
				int compare(const char *key) const;

				Iterator& find(const char *key);

				// Increment / Decrement
				Iterator& operator++();
				Iterator& operator--();

				Iterator operator++(int);
				Iterator operator--(int);

				// Arithmetic
				Iterator& operator+=(size_t off);
				Iterator operator+(size_t off) const;

				Iterator& operator-=(size_t off);
				Iterator operator-(size_t off) const;

				// Comparison operators
				bool operator== (const Iterator& b) const;
				bool operator!= (const Iterator& b) const;
				bool operator<  (const Iterator& b) const;
				bool operator<= (const Iterator& b) const;
				bool operator>  (const Iterator& b) const;
				bool operator>= (const Iterator& b) const;

			};

			Iterator begin() const;
			Iterator end() const;

			Iterator begin(const char *colname) const;

			/// @brief Build container from XML node.
			Container(const XML::Node &node);
			~Container();

			inline const char *id() const noexcept {
				return name;
			}

			/// @brief Get timestamp from source files.
			time_t last_modified() const;

			//Udjat::Value & get(size_t id, Udjat::Value &value) const;

			/// @brief Is the container loaded?
			bool loaded() const {
				return (bool) active_file;
			}

			virtual void state(const State state);

			/// @brief Get the number of entries in the container.
			size_t size() const;

			/// @brief Get column by name
			std::shared_ptr<Abstract::Column> column(const char *name) const;

			/// @brief Get column id from name.
			/// @return column index ou ((size_t) -1) if not found.
			size_t column_index(const char *name) const;

			/// @brief Get the timestamp of the last update.
			TimeStamp update_time() const;

			/// @brief Number of columns in the container.
			inline const std::vector<std::shared_ptr<Abstract::Column>> & columns() const noexcept {
				return cols;
			}

			/// @brief Load source files, rebuild work file.
			void load();

			/// @brief Get iterator using primary key or path.
			/// @param key The key to search on the primary index.
			/// @return The requested resource.
			const Iterator find(const char *key) const;

			/// @brief Get iterator using columns index.
			/// @param column The column name.
			/// @param key The key to search on the column index.
			/// @return The requested resource.
			const Iterator find(const char *column, const char *key) const;

			/// @brief Get value from search path.
			/// @param path The path for the required resource.
			/// @param value The value for resource data.
			/// @return true if value was updated.
			bool get(const char *path, Udjat::Value &value) const;

			/// @brief Get response from search path.
			/// @param path The path for the required resource.
			/// @param value The value for resource data.
			/// @return true if value was updated.
			bool get(const char *path, Udjat::Response::Value &value) const;

			/// @brief Get values from search path.
			/// @param path The path for the required resource.
			/// @param value The containser to responses.
			/// @return true if value was updated.
			bool get(const char *path, Udjat::Response::Table &value) const;

		protected:

			/// @brief Get record from index.
			/// @param offset The offset for the requested index.
			/// @param key The key to search.
			/// @param The path for required resource.
			/// @return The requested resource.
			// const Resource find(size_t offset, const char *key) const;

		};

	}

 }

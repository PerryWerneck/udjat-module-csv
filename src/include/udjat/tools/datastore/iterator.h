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
  * @brief Describe the datastore iterator.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/file.h>
 #include <udjat/tools/request.h>
 #include <udjat/tools/datastore/column.h>
 #include <udjat/tools/report.h>
 #include <udjat/tools/value.h>
 #include <memory>
 #include <functional>
 #include <vector>

 namespace Udjat {

 	namespace DataStore {

		class UDJAT_API Iterator {
		protected:

			/// @brief The current file holding the real data.
			const std::shared_ptr<File> file;

			/// @brief The data columns.
			const std::vector<std::shared_ptr<Abstract::Column>> cols;

			/// @brief Selected row (from 0 to the end of file)
			size_t row = 1;

			/// @brief Pointer to the index data.
			const size_t *ixptr = nullptr;

			/// @brief Index type (-1 = the default one).
			uint16_t ixtype = (uint16_t) -1;

			/// @brief Get pointer to selected row.
			const size_t * rowptr() const;

			/// @brief The filter expression.
			std::function<int(const Iterator &it)> filter;

			/// @brief Search using filter.
			void search();

			void set_default_filter(const std::string &search_key);

		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type   = int;
			using value_type        = Iterator;
			using pointer           = Iterator *;  // or also value_type*
			using reference         = Iterator &;  // or also value_type&

			/// @brief Build an iterator from path.
			static Iterator Factory(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const char *path);

			/// @brief Build an iterator from request.
			static Iterator Factory(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const Request &request);

			/// @brief Build an iterator to the entire file.
			Iterator(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols);

			/// @brief Build an iterator searching on primary key.
			Iterator(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const std::string &key);

			/// @brief Build and iterator searching a named column.
			Iterator(const std::shared_ptr<DataStore::File> file, const std::vector<std::shared_ptr<DataStore::Abstract::Column>> &cols, const std::string &column_name, const std::string &key);

			/// @brief Is this iterator valid?
			operator bool() const;

			/// @brief Set row.
			Iterator & operator=(const size_t row);

			// Increment / Decrement
			Iterator& operator++();
			Iterator& operator--();

			Iterator operator++(int);
			Iterator operator--(int);

			// Arithmetic
			Iterator& operator+=(size_t rows);
			Iterator operator+(size_t rows) const;

			Iterator& operator-=(size_t rows);
			Iterator operator-(size_t rows) const;

			// Comparison operators
			bool operator== (const Iterator& b) const;
			bool operator!= (const Iterator& b) const;
			bool operator<  (const Iterator& b) const;
			bool operator<= (const Iterator& b) const;
			bool operator>  (const Iterator& b) const;
			bool operator>= (const Iterator& b) const;

			// Get data

			/// @brief Get item count.
			size_t count() const;

			/// @brief Get primary key.
			std::string primary_key() const;

			/// @brief Get value from column name.
			std::string operator[](const char *name) const;

			/// @brief Get value from column number.
			std::string operator[](const size_t ix) const;

			/// @brief Get row contents.
			Udjat::Value & get(Udjat::Value &value) const;

			/// @brief Get values from iterator.
			/// @param path The path for the required resource.
			/// @param value The container to responses.
			/// @return true if value was updated.
			bool get(Udjat::Response::Table &value) const;

			bool head(Udjat::Abstract::Response &response) const;

		};


 	}

 }

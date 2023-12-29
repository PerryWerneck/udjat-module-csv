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
 #include <vector>
 #include <iterator>

 namespace Udjat {

	namespace DataStore {

		enum State : int {
			Undefined,		///< @brief Data store is in undefined state.
			Updating,		///< @brief Updating from data source.
			Failed,			///< @brief Update failed.
			Ready,			///< @brief Data was loaded from source.
			Empty			///< @brief Empty data.
		};

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

		public:

			/// @brief A resource inside the container.
			class Resource {
			private:
				friend class Container;

				std::shared_ptr<File> file;										///< @brief The data file
				const std::vector<std::shared_ptr<Abstract::Column>> cols;		///< @brief The column definitions.
				size_t *ixptr = nullptr;										///< @brief Pointer to the index block.
				size_t index = 0;												///< @brief Current record id;

				Resource(std::shared_ptr<File> file, const std::vector<std::shared_ptr<Abstract::Column>> &cols, size_t id);

				/// @brief Get pointer to row.
				virtual const size_t * recptr() const;

			public:
				~Resource();

				size_t count() const;

				Resource& set(size_t id = 0);

				inline Resource & operator=(size_t id) {
					return set(id);
				}

				operator bool() const;

				virtual std::string to_string() const;
				std::string operator[](const char *column) const;

				Udjat::Value & get(Udjat::Value &value) const;

				// Search
				/// @brief Is the resource 'similar' to key?
				inline bool operator== (const char *key) const {
					return compare(key) == 0;
				}

				/// @brief Compare resource with 'key'
				/// @param key the string to compare.
				/// @return result of the casecmp test.
				virtual int compare(const char *key) const;

				Resource& search(const char *key);

				// Increment / Decrement
				Resource& operator++();
				Resource& operator--();

				Resource operator++(int);
				Resource operator--(int);

				// Arithmetic
				Resource& operator+=(size_t off);
				Resource operator+(size_t off) const;

				Resource& operator-=(size_t off);
				Resource operator-(size_t off) const;

				// Comparison operators
				bool operator== (const Resource& b) const;
				bool operator!= (const Resource& b) const;
				bool operator<  (const Resource& b) const;
				bool operator<= (const Resource& b) const;
				bool operator>  (const Resource& b) const;
				bool operator>= (const Resource& b) const;

			};

			class Iterator : public Resource {
			private:
				friend class Container;
				Iterator(std::shared_ptr<File> file, const std::vector<std::shared_ptr<Abstract::Column>> &cols, size_t id);

			public:
				using iterator_category = std::random_access_iterator_tag;
				using difference_type   = size_t;
				using value_type        = Resource;
				using pointer           = Resource *;  // or also value_type*
				using reference         = Resource &;  // or also value_type&

				constexpr reference operator*() const noexcept { return (Resource &) *this; }
				constexpr pointer operator->() const noexcept { return (Resource *) this; }

				~Iterator();

			};

			Iterator begin() const;
			Iterator end() const;

			/// @brief Build container from XML node.
			Container(const XML::Node &node);
			~Container();

			/// @brief Get resource by id.
			inline Resource operator[](size_t id) {
				return Resource{active_file,cols,id};
			}

			inline Udjat::Value & get(size_t id, Udjat::Value &value) const {
				return Resource{active_file,cols,id}.get(value);
			}

			/// @brief Is the container loaded?
			bool loaded() const {
				return (bool) active_file;
			}

			virtual void state(const State state);

			/// @brief Get the number of entries in the container.
			size_t size() const;

			/// @brief Get the timestamp of the last update.
			TimeStamp update_time() const;

			/// @brief Number of columns in the container.
			inline const std::vector<std::shared_ptr<Abstract::Column>> & columns() const noexcept {
				return cols;
			}

			/// @brief Load source files, rebuild work file.
			void load();

			/// @brief Get record from path.
			/// @param key The key to search.
			/// @return The requested resource.
			const Resource find(const char *key) const;

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

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
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/timestamp.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/converters.h>
 #include <udjat/tools/response.h>
 #include <udjat/tools/report.h>
 #include <udjat/tools/request.h>
 #include <vector>
 #include <iterator>

 namespace Udjat {

 	namespace DataStore {

		enum State : uint8_t {
			Undefined,		///< @brief Data store is in undefined state.
			Updating,		///< @brief Updating from data source.
			Failed,			///< @brief Update failed.
			Ready,			///< @brief Data was loaded from source.
			Empty,			///< @brief Empty data.
			Waiting,		///< @brief Waiting to reload.
		};

		UDJAT_API DataStore::State StateFactory(const char *str);

		/// @brief A data store container.
		class UDJAT_API Container {
		private:

			const char *name;
			const char *path;

			time_t expires;

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

		protected:
			const char *filespec;

		public:

			/// @brief Build container from XML node.
			Container(const XML::Node &node);
			~Container();

			inline bool operator==(const char *name) const noexcept {
				return strcasecmp(name,this->name) == 0;
			}

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

			/// @brief Number of columns in the container.
			inline const std::vector<std::shared_ptr<Abstract::Column>> & columns() const noexcept {
				return cols;
			}

			/// @brief Load source files, rebuild work file.
			void load();

			/// @brief Get container by request.
			/// @param name The name of required datastore.
			/// @return nullptr if not found.
			static Container * get(const Request &request);

			/// @brief Get iterator using primary key or path.
			/// @param key The key to search on the primary index.
			/// @return Iterator with the first record found.
			Iterator find(const char *path) const;

			/// @brief Get iterator using columns index.
			/// @param column The column name.
			/// @param key The key to search on the column index.
			/// @return Iterator with the first record found.
			Iterator find(const char *column, const char *key) const;

			/// @brief Get iterator using request.
			/// @param request the request.
			Iterator find(Request &request);

			/*
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
			/// @param value The container to responses.
			/// @return true if value was updated.
			bool get(const char *path, Udjat::Response::Table &value) const;

			/// @brief Count number of responses.
			/// @param path The path for the required resource.
			bool head(const char *path, Udjat::Abstract::Response &response) const;

			/// @brief Count number of responses from iterator.
			/// @param it the iterator to count.
			/// @param path The path for the required resource.
			/// @return true if the iterator is valid.
			bool head(const DataStore::Iterator &it, Udjat::Abstract::Response &response) const;
			*/

		};

	}

 }

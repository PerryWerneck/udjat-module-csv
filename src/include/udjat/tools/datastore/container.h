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
 #include <vector>

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

			/// @brief Build container from XML node.
			Container(const XML::Node &node);
			~Container();

			virtual void state(const State state);

			/// @brief Number of columns in the container.
			inline const std::vector<std::shared_ptr<Abstract::Column>> & columns() const noexcept {
				return cols;
			}

			/// @brief Load source files, rebuild work file.
			void load();

		};

	}

 }

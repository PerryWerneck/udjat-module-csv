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

 namespace Udjat {

	namespace DataStore {

		/// @brief A data store container.
		class UDJAT_API Container {
		private:

			/// @brief The current file holding the real data.
			std::shared_ptr<File> active_file;

			/// @brief The data columns.
			std::vector<std::shared_ptr<Abstract::Column>> columns;

		public:

			/// @brief Build container from XML node.
			Container(const XML::Node &node);
			~Container();
		};


		/*
		namespace Source {

			/// @brief Abstract data source.
			class UDJAT_API Abstract {
			protected:

				/// @brief The source columns.
				std::vector<std::shared_ptr<Abstract::Column>> columns;

			public:
				/// @brief Build data source from XML definitions.
				Source(const XML::Node &definition);
				virtual ~Source();

				/// @brief Load from data source.
				// virtual void load() = 0;

			};

			/// @brief Source from CSV files.
			class UDJAT_API CSV : public Abstract {
			private:
				CSV(const XML::Node &definition);
				virtual ~CSV();

			};

		}
		*/

	}

 }

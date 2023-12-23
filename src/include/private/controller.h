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
  * @brief Declare table controller.
  */

 #pragma once
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/memdb/simpletable.h>
 #include <thread>

 namespace Udjat {

	namespace MemCachedDB {

		class UDJAT_PRIVATE DataFile {
		private:
			int fd = -1;				///< @brief Handle of the real data file.
			uint8_t * ptr = nullptr;	///< @brief Pointer to memory mapped block.
			std::recursive_mutex guard;
		public:
			DataFile();
			DataFile(const char *filename);
			~DataFile();

			size_t size();

			/// @brief Append data on file.
			/// @param data The datablock to write.
			/// @param length The length of block.
			/// @return The offset of the data.
			size_t write(const void *data, size_t length);

			/// @brief Read data from file.
			/// @param offset Offset of the required data.
			/// @param data The pointer to store data.
			/// @param length The length of data.
			void read(size_t offset, void *data, size_t length);

		} datafile;

		/// @brief The table controller, manage loading and memmapped data.
		class UDJAT_PRIVATE Table::Controller {
		public:
			enum State : uint8_t {
				Ready,			///< @brief Table is loaded and ready to use.
				Unavailable,	///< @brief Table is unavailable.
				Loading,		///< @brief Table is loading.
				Failed,			///< @brief Table load has failed.
			} current_state = Unavailable;

			std::shared_ptr<DataFile> datafile;

			/// @brief Build and empty controller.
			Controller() { }

			/// @brief Build csv from XML node.
			Controller(const XML::Node &node);
			~Controller();

		private:

			/// @brief The file/folder to look for source files.
			const char *path = nullptr;

			/// @brief Load csv files.
			void reload();

		};

	}


 }

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
  * @brief Declare raw file
  */

 #pragma once
 #include <udjat/defs.h>
 #include <mutex>
 #include <string>

 namespace Udjat {

	namespace DataStore {

		/// @brief Handle a datastore file.
		class UDJAT_API File {
		private:

#ifdef _WIN32
			std::string tempfilename;
#endif // _WIN32

			int fd = -1;				///< @brief Handle of the real data file.
			uint8_t * ptr = nullptr;	///< @brief Pointer to memory mapped block.
			std::mutex guard;


		public:
			File();
			File(const char *filename);
			~File();

			size_t size();

			void map();
			void unmap();

			const void * get_void_ptr(size_t offset) const;

			template <typename T>
			inline const T & get(size_t offset) const {
				return *((T *) get_void_ptr(offset));
			}

			template <typename T>
			inline const T * get_ptr(size_t offset) const {
				return (T *) get_void_ptr(offset);
			}

			/// @brief Is the file mapped?
			/// @return true if the file is mapped.
			bool mapped() const noexcept {
				return (bool) ptr;
			}

			/// @brief Write data to file.
			/// @param offset for start.
			/// @param data The datablock to write.
			/// @param length The length of block.
			void write(size_t offset, const void *data, size_t length);

			template <typename T>
			inline void write(size_t offset, const T &value) {
				return write(offset, &value,sizeof(T));
			}

			/// @brief Append data on file.
			/// @param data The datablock to write.
			/// @param length The length of block.
			/// @return The offset of the data.
			size_t write(const void *data, size_t length);

			template <typename T>
			inline size_t write(const T &value) {
				return write(&value,sizeof(T));
			}

			/// @brief Write string on file.
			/// @param str the string to write.
			/// @return The offset of the data.
			size_t write(const char *data);

			/// @brief Read data from file.
			/// @param offset Offset of the required data.
			/// @param data The pointer to store data.
			/// @param length The length of data.
			void read(size_t offset, void *data, size_t length);

			/// @brief Read string from file.
			/// @param offset Offset of the required text.
			/// @return The string at offset.
			std::string read(size_t offset);
		};

	}

 }

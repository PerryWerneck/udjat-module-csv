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
  * @brief Brief description of this source.
  */

 #pragma once
 #include <udjat/defs.h>

 namespace Udjat {

	namespace DataStore {

		#pragma pack(1)
		/// @brief File header.
		struct Header {
			time_t updated;			///< @brief Timestamp of the last update.
			size_t primary_offset;	///< @brief Offset for the beginning of the primary index.
			size_t columns;			///< @brief Number of columns.
			struct {
				size_t count;		///< @brief Count of secondary indexes.
				size_t offset;
			} indexes;
		};
		#pragma pack()

		#pragma pack(1)
		/// @brief Index list item.
		struct Index {
			uint16_t column;		///< @brief Column id for the index.
			size_t offset;			///< @brief Offset for index elements.
		};
		#pragma pack()

	}

 }

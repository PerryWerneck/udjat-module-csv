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

 #include <config.h>
 #include <udjat/defs.h>
 #include <private/loader.h>
 #include <cstring>

 #pragma pack(1)
 struct Header {
	uint8_t dunno;
 };
 #pragma pack()


 Loader::Loader(std::shared_ptr<MemCachedDB::File> f) : file{f} {

	if(!file->size()) {
		Header hdr;
		memset(&hdr,0,sizeof(hdr));
		file->write(&hdr,sizeof(hdr));
	}

 }

 Loader::~Loader() {

 }

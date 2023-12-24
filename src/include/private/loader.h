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
  * @brief Declare file loader..
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/memdb/file.h>
 #include <memory>
 #include <unordered_set>

 using namespace Udjat;

 /// @brief Load CSV file into MemCache file.
 class UDJAT_PRIVATE Loader {
 private:

 	///< @brief The file to store data.
	std::shared_ptr<MemCachedDB::File> file;

	/// @brief Data block
	class Block {
	private:
		std::shared_ptr<MemCachedDB::File> file;		///< @brief The data file.
		size_t length;									///< @brief Length of the datablock.
		size_t offset;									///< @brief The offset of the datablock.
		size_t hash;									///< @brief Hash of the datablock

	public:
		Block(std::shared_ptr<MemCachedDB::File> file, void *data, size_t length);
		bool operator==(const Block &b) const;

		struct HashFunction {
			size_t operator()(const Block& block) const {
				return block.hash;
			}
		};

	};

	std::unordered_set<Block, Block::HashFunction> blocks;

	/// @brief Insert data block in file avoiding duplication.
	/// @return The data offset.
	size_t insert(void *data, size_t length);

 public:
	Loader(std::shared_ptr<MemCachedDB::File> f);
	~Loader();

 };

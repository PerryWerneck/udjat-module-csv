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
 #include <udjat/tools/datastore/file.h>
 #include <memory>
 #include <unordered_set>
 #include <mutex>
 #include <cstring>

 namespace Udjat {

	namespace DataStore {

		/// @brief Loader tool. Store data on file without duplication.
		class UDJAT_API Deduplicator {
		private:

			///< @brief Serialization
			std::mutex guard;

			///< @brief The file to store data.
			std::shared_ptr<File> file;

			/// @brief Data block
			class Block {
			public:
				std::shared_ptr<DataStore::File> file;		///< @brief The data file.
				size_t length = 0;							///< @brief Length of the datablock.
				size_t offset = 0;							///< @brief The offset of the datablock.
				size_t hash = 0;							///< @brief Hash of the datablock

				virtual bool compare(const void *src) const;

				Block(std::shared_ptr<DataStore::File> file, const void *data, size_t length);
				virtual bool operator==(const Block &b) const;

				struct HashFunction {
					size_t operator()(const Block& block) const {
						return block.hash;
					}
				};

			};

			std::unordered_set<Block, Block::HashFunction> blocks;

		public:

			/// @brief Construct a data storage to file.
			/// @param f The file to storage data.
			Deduplicator(std::shared_ptr<File> f) : file{f} {
			}

			/// @brief Insert data block in file avoiding duplication.
			/// @return The data offset.
			size_t insert(const void *data, size_t length);

			/// @brief Insert an string in file avoiding duplication.
			/// @return The data offset.
			inline size_t insert(const char *str) {
				return insert(str,strlen(str)+1);
			}

		};

	}

 }

 /*
 namespace Udjat {


	namespace MemCachedDB {

		/// @brief Load CSV file into MemCache file.
		class UDJAT_PRIVATE Table::Loader {
		private:

			///< @brief The file to store data.
			std::shared_ptr<MemCachedDB::File> file;

			/// @brief Data block
			class Block {
			private:
				std::shared_ptr<MemCachedDB::File> file;		///< @brief The data file.
				size_t length = 0;								///< @brief Length of the datablock.
				size_t offset = 0;								///< @brief The offset of the datablock.
				size_t hash = 0;								///< @brief Hash of the datablock

			protected:

				virtual bool compare(void *src) const;

			public:
				Block(std::shared_ptr<MemCachedDB::File> file, const void *data, size_t length);
				virtual bool operator==(const Block &b) const;

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
			Loader();
			Loader(const char *dbname);
			Loader(std::shared_ptr<MemCachedDB::File> file);
			~Loader();

			/// @brief Load file(s).
			/// @param path Filename/path for the table definition.
			void load(const char *path);

		};
	}
 }
 */

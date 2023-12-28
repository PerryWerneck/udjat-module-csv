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
  * @brief Declare classes to load a datastore from sources.
  */

 #pragma once

 #include <udjat/defs.h>
 #include <memory>
 #include <udjat/tools/datastore/deduplicator.h>
 #include <udjat/tools/datastore/file.h>
 #include <udjat/tools/datastore/container.h>
 #include <string>
 #include <sys/types.h>
 #include <sys/stat.h>

 namespace Udjat {

	namespace DataStore {

		namespace Abstract {

			class UDJAT_API Loader {
			protected:

				const Container &container;

				struct InputFile {
					std::string name;
					struct stat st;
				};
				std::vector<InputFile> files;

				/// @brief Loading Context
				class Context {
				private:
					Deduplicator &deduplicator;

				public:
					Context(Deduplicator &d) : deduplicator{d} {
					}


				};

			public:

				Loader(const DataStore::Container &container, const char *path, const char *filespec);

				inline bool empty() const noexcept {
					return files.empty();
				}

				void load();

			};

		}

	}

 }

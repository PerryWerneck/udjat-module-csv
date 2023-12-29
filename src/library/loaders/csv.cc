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
  * @brief Implements datastore loader.
  */
 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/loader.h>
 #include <udjat/tools/string.h>
 #include <fstream>
 #include <vector>
 #include <string>
 #include <udjat/tools/logger.h>

 using namespace std;

 namespace Udjat {

	static void split(const std::string &string, std::vector<String> &columns, const char delimiter = ';') {

		columns.clear();
		const char *ptr = string.c_str();

		while(*ptr) {

			if(*ptr == '\"') {

				// It's an string delimited by "
				ptr++;
				const char *to = strchr(ptr,'\"');
				if(!to) {
					throw runtime_error("Bad file, mismatch on '\"' delimiter");
				}
				columns.push_back(std::string{ptr,(size_t) (to-ptr)});
				ptr = strchr(to,delimiter);
				if(!ptr) {
					return;
				}
				ptr++;
			} else {
				const char *to = strchr(ptr,';');
				if(!to) {
					columns.emplace_back(ptr);
					return;
				}

				columns.push_back(std::string{ptr,(size_t) (to-ptr)});
				ptr = to+1;

			}

			while(*ptr && isspace(*ptr)) {
				ptr++;
			}

		}
	}

	void DataStore::Loader::CSV::load_file(Context &context, const char *filename) {

		std::ifstream infile{filename};

		String line;

		// Read first line to get field names.
		{
			std::getline(infile, line);
			std::vector<String> headers;
			split(line.strip(), headers,';');
			context.open(headers);
		}

		// Read csv contents.
		while(std::getline(infile, line)) {

			line.strip();
			if(line.empty()) {
				Logger::String{"Stopping on empty line"}.info("csvloader");
				break;
			}

			std::vector<String> cols;
			split(line.strip(), cols,';');
			context.append(cols);

		}

	}

 }


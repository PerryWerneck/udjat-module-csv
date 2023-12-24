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

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/module.h>
 #include <udjat/worker.h>
 #include <udjat/factory.h>
 #include <udjat/tools/request.h>
 #include <udjat/tools/response.h>
 #include <stdexcept>


 using namespace Udjat;
 using namespace std;

 static const ModuleInfo module_info{"csvdb", "Simple read-only CSV based database"};

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	class Module : public Udjat::Module, Udjat::Worker, private Udjat::Factory {
	public:
		Module() : Udjat::Module("csv-file",module_info), Udjat::Worker("csv",module_info), Udjat::Factory("csv-file",module_info) {
		}

		~Module() {
		}

		bool work(Request &request, Response::Value &response) const override {
			return false;
		}

		// Udjat::Factory
		bool generic(const pugi::xml_node &node) override {

			return false;
		}

	};

	return new Module();

 }
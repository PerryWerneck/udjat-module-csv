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
 #include <udjat/agent.h>
 #include <stdexcept>
 #include <vector>
 #include <udjat/tools/memdb/simpletable.h>
 #include <udjat/tools/threadpool.h>

 using namespace Udjat;
 using namespace std;

 static const ModuleInfo module_info{"csvdb", "Simple read-only CSV based database"};

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	class Module : public Udjat::Module, Udjat::Worker, private Udjat::Factory {
	private:

		class Table : public Agent<MemCachedDB::Table::State>, public MemCachedDB::Table {
		private:

		public:
			Table(const pugi::xml_node &definition) : Agent<MemCachedDB::Table::State>{definition,MemCachedDB::Table::Undefined}, MemCachedDB::Table{definition} {
				debug("-----> Building table");
			}

			void state(const MemCachedDB::Table::State state) noexcept override {
				super::set(state);
			}

			MemCachedDB::Table::State state() const noexcept override {
				return super::get();
			}

			void start() override {

				// TODO: Load default states.

				// Load table contents in background thread to avoid 'hangs' while starting application.
				push([](std::shared_ptr<Abstract::Agent> me){
					((Table *)me.get())->load();
				});

				Abstract::Agent::start();
			}

		};

		vector<Table> tables;

	public:
		Module() : Udjat::Module("csv-file",module_info), Udjat::Worker("csv",module_info), Udjat::Factory("csv-file",module_info) {
		}

		~Module() {
		}

		bool work(Request &request, Response::Value &response) const override {
			return false;
		}

		// Udjat::Factory
		std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &, const pugi::xml_node &node) const {
			return make_shared<Table>(node);
		}

		/*
		bool generic(const pugi::xml_node &node) override {
			tables.emplace_back(node);
			return true;
		}
		*/

	};

	return new Module();

 }

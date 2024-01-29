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
 #include <udjat/tools/worker.h>
 #include <udjat/factory.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/request.h>
 #include <udjat/tools/abstract/response.h>
 #include <udjat/agent/datastore.h>
 #include <udjat/tools/datastore/container.h>
 #include <stdexcept>

 using namespace Udjat;
 using namespace std;

 static const ModuleInfo module_info{"csvdb", "Simple read-only CSV based database"};

 /// @brief Register udjat module.
 Udjat::Module * udjat_module_init() {

	class Module : public Udjat::Module, Udjat::Worker, private Udjat::Factory {
	private:

	public:
		Module() : Udjat::Module("csv",module_info), Udjat::Worker("csv",module_info), Udjat::Factory("csv",module_info) {
		}

		~Module() {
		}

		Worker::ResponseType probe(const Request &request) const noexcept override {

			debug(__FUNCTION__,"----------------------------------(",request.path(),")");
			if(DataStore::Container::get(request)) {
				debug("Accepting request '",request.path(),"'");
				return ResponseType::Table;
			}

			debug("Rejecting request '",request.path(),"'");
			return ResponseType::None;
		}

		bool work(Request &request, Response::Table &response) const override {

			debug(__FUNCTION__,"('",request.path(),"')");

			auto db = DataStore::Container::get(request);
			if(!db) {
				return false;
			}

			{
				time_t timestamp = db->last_modified();
				response.last_modified(timestamp);
				if(request.cached(timestamp)) {
					response.not_modified(true);
					return true;
				}
			}

			request.pop();	// Remove db name.
			DataStore::Iterator it = db->find(request);
			if(!it) {
				return false;
			}

			if( ((HTTP::Method) request) == HTTP::Get) {

				debug("HTTP GET");
				it.get(response);

			} else if( ((HTTP::Method) request) == HTTP::Head) {

				debug("HTTP HEAD");
				return it.head(response);

			} else {

				return false;

			}

			return true;

		}

		// Udjat::Factory
		std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &, const pugi::xml_node &node) const {
			return make_shared<DataStore::Agent>(node);
		}

	};

	return new Module();

 }

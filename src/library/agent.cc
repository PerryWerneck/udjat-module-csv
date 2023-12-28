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
  * @brief Implements datastore agent.
  */

 #include <config.h>
 #include <udjat/tools/intl.h>
 #include <udjat/agent/datastore.h>
 #include <memory>

 using namespace std;

 namespace Udjat {

	static const char *state_names[] = {
		"Undefined",
		"Loading",
		"Failed",
		"Loaded",
		"Empty"
	};

	DataStore::Agent::Agent(const XML::Node &definition)
		: Udjat::Agent<DataStore::State>(definition,DataStore::Undefined), Udjat::DataStore::Container{definition} {
	}

	DataStore::Agent::~Agent() {
	}

	void DataStore::Agent::start() {

		//push([](std::shared_ptr<Udjat::Abstract::Agent> me) {
		//	((DataStore::Agent *)me.get())->load();
		//});

		Udjat::Agent<DataStore::State>::start(Updating);

	}

	std::string DataStore::Agent::to_string() const noexcept {

		int value = (int) get();

		if(value >= 0 && value < (int) ((sizeof(state_names)/sizeof(state_names[0])))) {
			return state_names[value];
		}

		return Logger::Message{_("Unexpected value '{}'"),value};

	}

	std::shared_ptr<Abstract::State> DataStore::Agent::StateFactory(const XML::Node &node) {

		const char *name = node.attribute("value").as_string(node.attribute("name").as_string(""));
		if(name && *name) {
			for(size_t ix = 0; ix < (sizeof(state_names)/sizeof(state_names[0])); ix++) {
				if(!strcasecmp(name,state_names[ix])) {
					debug("Creating state '",state_names[ix],"'");
					auto state = make_shared<Udjat::State<DataStore::State>>(node,(DataStore::State) ix);
					states.push_back(state);
					return state;
				}
			}
		}

		return super::StateFactory(node);

	}

 }


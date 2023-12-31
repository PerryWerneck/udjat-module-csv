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
 #include <udjat/agent.h>
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

	void DataStore::Agent::state(const DataStore::State state) {
		Udjat::Agent<DataStore::State>::set(state);
	}

	bool DataStore::Agent::getProperty(const char *key, std::string &value) const {

		if(loaded()) {

			// Has data, process it.
			if(!strcasecmp(key,"records")) {
				value = std::to_string(size());
			}

		}

		return super::getProperty(key,value);
	}

	Value & DataStore::Agent::getProperties(Value &value) const {

		if(loaded()) {

			// Has data, process it.
			value["records"] = size();
			value["dbupdate"] = update_time();

		} else {

			value["records"] = 0;
			value["dbupdate"] = 0;

		}

		return super::getProperties(value);
	}

	void DataStore::Agent::start() {

		Udjat::Agent<DataStore::State>::start(Undefined);

		push([](std::shared_ptr<Udjat::Abstract::Agent> me) {
			((DataStore::Agent *)me.get())->load();
		});

	}

	std::string DataStore::Agent::to_string() const noexcept {

		int value = (int) super::get();

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

	bool DataStore::Agent::getProperties(const char *path, Value &value) const {

		if(super::getProperties(path,value)) {
			return true;
		}

		return DataStore::Container::get(path,value);

		/*

		if(!strncasecmp(path,"id/",3)) {

			// It's an id
			DataStore::Container::get((size_t) atoi(path+3)-1, value);

		} else if(!strchr(path,'/')) {

			debug("Searching for primary key '",path,"'");
			Resource res{DataStore::Container::find(path)};

			if(res) {
				debug("Found resource '",res.to_string(),"'");
				res.get(value);
				return true;
			}

			return false;

		} else {

			// TODO: Search in the format [Column-name]/[Column-value]


			return false;

		}

		return true;

		*/

	}

 }


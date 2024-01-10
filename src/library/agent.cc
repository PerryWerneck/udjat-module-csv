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
 #include <udjat/agent/abstract.h>
 #include <udjat/agent.h>
 #include <udjat/tools/file/watcher.h>
 #include <udjat/tools/datastore/container.h>
 #include <udjat/agent/datastore.h>
 #include <udjat/tools/configuration.h>
 #include <memory>
 #include <regex>

 using namespace std;

 namespace Udjat {

	static const char *state_names[] = {
		"Undefined",
		"Loading",
		"Failed",
		"Loaded",
		"Empty",
		"Waiting"
	};

	UDJAT_API DataStore::State DataStore::StateFactory(const char *str) {

		if(str && *str) {
			for(size_t ix = 0; ix < N_ELEMENTS(state_names); ix++) {
				if(!strcasecmp(str,state_names[ix])) {
					return (DataStore::State) ix;
				}
			}
		}

		throw runtime_error(Logger::String{"Unexpected state name '",str,"'"});

	}

	DataStore::Agent::Agent(const XML::Node &definition)
		: Udjat::Agent<DataStore::State>(definition,DataStore::Undefined),
		  Udjat::DataStore::Container{definition},
		  Udjat::File::Watcher{definition,"sources-from" } {
	}

	DataStore::Agent::~Agent() {
	}

	void DataStore::Agent::updated(const Udjat::File::Watcher::Event, const char *filename) {

		if(filename && *filename) {
			auto pattern = std::regex(filespec,std::regex::icase);
			if(!std::regex_match(filename,pattern)) {
				trace() << "Ignoring change of '" << filename << "', doesnt match '" << filespec << "'" << endl;
				return;
			}
		}

		reload_required = true;

		// Wait a few seconds.
		TimeStamp next{sched_update(Config::Value<time_t>{"file-agent","auto-update-delay",30}.get())};

		trace() << "Agent update scheduled to " << next << " due to changes on '" << filename << "'" << endl;

		state(DataStore::Waiting);

	}

	void DataStore::Agent::state(const DataStore::State state) {
		debug("State set to '",state_names[state],"' (",(int) state,")");
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

		debug("--------------------------------> ",__FUNCTION__);

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

	bool DataStore::Agent::getProperties(const char *path, Response::Value &value) const {

		debug("path='",path,"'");
		if(super::getProperties(path,value)) {
			debug("Got response from parent object");
			return true;
		}

		debug("Getting response from container");
		return DataStore::Container::get(path,value);

	}

	bool DataStore::Agent::getProperties(const char *path, Response::Table &value) const {

		debug("path='",path,"'");
		if(super::getProperties(path,value)) {
			debug("Got response from parent object");
			return true;
		}

		debug("Getting response from container");
		return DataStore::Container::get(path,value);
		return false;

	}

	bool DataStore::Agent::refresh() {

		if(reload_required) {
			reload_required = false;
			load();
			return true;
		}

		return false;
	}

	bool DataStore::Agent::getProperties(const char *path, Value &value) const {

		debug("path='",path,"'");
		if(super::getProperties(path,value)) {
			debug("Got response from parent object");
			return true;
		}

		debug("Getting response from container");
		return DataStore::Container::get(path,value);

	}

 }


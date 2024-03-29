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
  * @brief Declare an udjat agent to manage a datastore.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/agent.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/file/watcher.h>
 #include <udjat/tools/datastore/container.h>
 #include <iostream>

 using namespace std;

 namespace Udjat {

	template <>
	inline DataStore::State from_xml<DataStore::State>(const XML::Node &node, const DataStore::State, const char *attrname) {
		const char *name = node.attribute(attrname).as_string("");
		if(!*name) {
			name = node.attribute("name").as_string("undefined");
		}
		return DataStore::StateFactory(name);
	}

	namespace DataStore {

		/// @brief Data store default agent.
		class UDJAT_API Agent : public Udjat::Agent<DataStore::State>, public Udjat::DataStore::Container, private Udjat::File::Watcher {
		private:

			bool reload_required = false;	///< @brief True if there is a pending file watch event.
			void updated(const Udjat::File::Watcher::Event event, const char *filename) override;

		protected:

		public:
			Agent(const XML::Node &definition);
			virtual ~Agent();

			void state(const DataStore::State state) override;

			void start() override;

			bool refresh() override;

			std::string to_string() const noexcept override;

			std::shared_ptr<Udjat::Abstract::State> StateFactory(const XML::Node &node) override;

			bool getProperty(const char *key, std::string &value) const override;

			Value & getProperties(Value &value) const override;

			bool getProperties(const char *path, Value &value) const override;
			bool getProperties(const char *path, Response::Value &value) const override;
			bool getProperties(const char *path, Response::Table &value) const override;

		};

	}

 }

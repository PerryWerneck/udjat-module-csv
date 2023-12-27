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
 #include <udjat/tools/application.h>
 #include <udjat/module.h>
 #include <unistd.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/memdb/simpletable.h>

 #include <udjat/tools/memdb/datastore.h>

 using namespace std;
 using namespace Udjat;

 int main(int argc, char **argv) {

	Logger::verbosity(9);
	Logger::redirect();
	Logger::console(true);

	/*
	udjat_module_init();

	auto rc = Application{}.run(argc,argv,"./test.xml");

	debug("Application exits with rc=",rc);

	return rc;
	*/

	debug("Starting test");
	std::shared_ptr<MemCachedDB::File> file{make_shared<MemCachedDB::File>("/tmp/datastore.db")};
	DataStore db{file};

	file->write("\0",1);

	cout << db.insert("teste1") << endl;
	cout << db.insert("teste2") << endl;
	cout << db.insert("teste3") << endl;

	cout << db.insert("teste1") << endl;
	cout << db.insert("teste2") << endl;
	cout << db.insert("teste3") << endl;
}

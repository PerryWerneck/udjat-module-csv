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
  * @brief Implements iterator handler.
  */

 #include <config.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <udjat/tools/datastore/file.h>
 #include <private/iterator.h>
 #include <udjat/tools/logger.h>
 #include <stdexcept>
 #include <private/structs.h>

 using namespace std;

 namespace Udjat {

	DataStore::Iterator::Handler::Handler() {
	}

	DataStore::Iterator::Handler::~Handler() {
	}

	uint16_t DataStore::Iterator::Handler::search_column_id(const Iterator &it, const char *colname) {

		for(size_t c = 0; c < it.cols.size();c++) {
			if(it.cols[c]->indexed() && !strcasecmp(it.cols[c]->name(),colname)) {
				return (uint16_t) c;
			}
		}

		throw runtime_error(Logger::String{"Unable to search using '",colname,"'"});

	}

	DataStore::PrimaryKeyHandler::PrimaryKeyHandler(const Iterator &it, const char *s) : search_key{s} {

		// Get pointer to primary index.
		ixptr = file(it)->get_ptr<size_t>(file(it)->get<Header>(0).primary_offset);

	}

	DataStore::PrimaryKeyHandler::~PrimaryKeyHandler() {
	}

	DataStore::ColumnKeyHandler::~ColumnKeyHandler() {
	}

	DataStore::ColumnKeyHandler::ColumnKeyHandler(const Iterator &it, uint16_t c, const char *search_key) : PrimaryKeyHandler{it,search_key}, colnumber{c} {

		// Get pointer to column index.
		const Header &header{file(it)->get<Header>(0)};
		const Index *index{file(it)->get_ptr<Index>(header.indexes.offset)};

		ixptr = nullptr;
		for(size_t ix = 0;ix < header.indexes.count;ix++) {

			if(index->column == colnumber) {
				ixptr = file(it)->get_ptr<size_t>(index->offset);
				break;
			}
			index++;
		}

		if(!ixptr) {
			throw logic_error(Logger::String{"Unable to find index for required column"});
		}

	}

	DataStore::ColumnKeyHandler::ColumnKeyHandler(const Iterator &it, const char *colname, const char *search_key)
		: ColumnKeyHandler{it,search_column_id(it,colname),search_key} {
	}

	size_t DataStore::PrimaryKeyHandler::size() const {
		return ixptr[0];
	}

	void DataStore::PrimaryKeyHandler::key(const char *key) {
		search_key = key;
	}


	const size_t * DataStore::PrimaryKeyHandler::rowptr(const Iterator &it) const {

		if(row(it) > ixptr[0]) {
			throw runtime_error(Logger::String{"Invalid row, should be from 0 to ",(int) ixptr[0]});
		}

		return ixptr + 1 + (row(it) * cols(it).size());

	}

	const size_t * DataStore::ColumnKeyHandler::rowptr(const Iterator &it) const {

		if(row(it) > ixptr[0]) {
			throw runtime_error(Logger::String{"Invalid row, should be from 0 to ",(int) ixptr[0]});
		}

		return file(it)->get_ptr<size_t>( *(ixptr + 1 + row(it)) );

	}

	int DataStore::PrimaryKeyHandler::filter(const Iterator &it) const {

		const char *key = search_key.c_str();

		const size_t *row{rowptr(it)};
		for(const auto col : cols(it)) {

			if(col->key()) {

				// It's a primary column, test it.
				string value{col->to_string(file(it),row)};
				col->apply_layout(value);

				debug("col: '",value,"' key: '",key,"'");

				size_t keylen = strlen(key);

				if(keylen < value.size()) {

					// The query string is smaller than the column, do a partial test.
					return strncasecmp(value.c_str(),key,keylen);

				} else {

					// The query string is equal or larger than the column, do a full test.
					int rc = strncasecmp(value.c_str(),key,value.size());
					if(rc) {
						return rc;
					}

				}

				// Get next block.
				key += value.size();
				if(!*key) {
					// Key is complete, found it.
					return 0;
				}

			}
		}

		return 1;

	}

	int DataStore::ColumnKeyHandler::filter(const Iterator &it) const {
		return cols(it)[colnumber]->comp(file(it),rowptr(it),search_key.c_str());
	}

	void DataStore::CustomKeyHandler::push_back(const Iterator &it) {
		records.push_back(handler(it)->rowptr(it));
	}

	const size_t * DataStore::CustomKeyHandler::rowptr(const Iterator &it) const {

		size_t row{this->row(it)};

		if(row >= records.size()) {
			throw runtime_error(Logger::String{"Invalid row ",row,", should be from 0 to ",(int) (records.size()-1)});
		}

		debug("row=",row," rowptr=",((size_t)records[row])," max=",records.size());

		return records[row];

	}

	int DataStore::CustomKeyHandler::filter(const Iterator &) const {
		return 0;
	}

	size_t DataStore::CustomKeyHandler::size() const {
		return records.size();
	}

	void DataStore::CustomKeyHandler::key(const char *) {
		throw logic_error("Cant set key on custom handler");
	}

	/*
	void DataStore::Iterator::set_default_index(const std::string &search_key) {

		if(ixtype != (uint16_t) -1) {

			// Use column based filter.
			filter = [search_key](const Iterator &it) {
				return it.cols[it.ixtype]->comp(it.file,it.rowptr(),search_key.c_str());
			};


		} else {

			// Use primary key filter.
			filter = [search_key](const Iterator &it) {

				const char *key = search_key.c_str();

				const size_t *row{it.rowptr()};
				for(const auto col : it.cols) {

					if(col->key()) {

						// It's a primary column, test it.
						string value{col->to_string(it.file,row)};
						col->apply_layout(value);

						debug("col: '",value,"' key: '",key,"'");

						size_t keylen = strlen(key);

						if(keylen < value.size()) {

							// The query string is smaller than the column, do a partial test.
							return strncasecmp(value.c_str(),key,keylen);

						} else {

							// The query string is equal or larger than the column, do a full test.
							int rc = strncasecmp(value.c_str(),key,value.size());
							if(rc) {
								return rc;
							}

						}

						// Get next block.
						key += value.size();
						if(!*key) {
							// Key is complete, found it.
							return 0;
						}

					}
				}

				return 1;

			};

		}

		search();	// Select first entry.
	}
	*/

 }

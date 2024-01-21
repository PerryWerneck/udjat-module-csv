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
  * @brief Declare iterator index.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/datastore/iterator.h>
 #include <vector>

 namespace Udjat {

 	namespace DataStore {

		class UDJAT_API Iterator::Handler {
		protected:

			inline auto row(const Iterator &it) const noexcept {
				return it.row;
			}

			inline auto cols(const Iterator &it) const noexcept {
				return it.cols;
			}

			inline auto file(const Iterator &it) const noexcept {
				return it.file;
			}

			inline auto handler(const Iterator &it) const noexcept {
				return it.handler;
			}

		public:
			Handler();
			virtual ~Handler();

			/// @brief Convenience method to get column number from name.
			static uint16_t search_column_id(const Iterator &it, const char *colname);

			/// @brief Get pointer to selected row.
			virtual const size_t * rowptr(const Iterator &it) const = 0;

			/// @brief The filter expression.
			virtual int filter(const Iterator &it) const = 0;

			/// @brief Get Record count;
			virtual size_t size() const = 0;

			/// @brief Set search key.
			virtual void key(const char *key) = 0;

		};

		/// @brief Handler for primary key index.
		class UDJAT_API PrimaryKeyHandler : public Iterator::Handler {
		protected:

			/// @brief The search key
			std::string search_key;

			/// @brief Pointer to the index data.
			const size_t *ixptr = nullptr;

		public:
			PrimaryKeyHandler(const Iterator &it, const char *search_key = "");
			virtual ~PrimaryKeyHandler();

			const size_t * rowptr(const Iterator &it) const override;
			int filter(const Iterator &it) const override;
			size_t size() const override;
			void key(const char *key) override;

		};

		/// @brief Handler for column based index.
		class UDJAT_API ColumnKeyHandler : public PrimaryKeyHandler {
		private:

			/// @brief Key column number.
			uint16_t colnumber;

		public:
			ColumnKeyHandler(const Iterator &it, uint16_t colnumber, const char *search_key = "");
			ColumnKeyHandler(const Iterator &it, const char *colname, const char *search_key = "");

			virtual ~ColumnKeyHandler();

			const size_t * rowptr(const Iterator &it) const override;
			int filter(const Iterator &it) const override;

		};

		/// @brief Handler for primary key index.
		class UDJAT_API CustomKeyHandler : public Iterator::Handler {
		protected:

			/// @brief The selected records.
			std::vector<const size_t *> records;

		public:
			CustomKeyHandler() = default;

			void push_back(const Iterator &it);

			const size_t * rowptr(const Iterator &it) const override;
			int filter(const Iterator &it) const override;
			size_t size() const override;
			void key(const char *key) override;


		};

 	}

 }

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
  * @brief Implement the basic DataFile methods.
  */

 #include <config.h>
 #include <private/controller.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <stdexcept>
 #include <sys/mman.h>

 //#ifdef HAVE_UNISTD_H
	#include <unistd.h>
 //#endif // HAVE_UNISTD_H

 #include <sys/mman.h>

 using namespace std;

 namespace Udjat {

	MemCachedDB::DataFile::DataFile() : fd{open("/tmp",O_RDWR|O_CREAT|O_TMPFILE,0640)} {
		if(fd < 0) {
			throw std::system_error(errno,std::system_category(),"Unable to create temporary file");
		}
		const char *pname = PACKAGE_NAME;
		write(pname,strlen(pname)+1);
	}

	MemCachedDB::DataFile::DataFile(const char *filename) : fd{::open(filename,O_RDWR|O_CREAT,0640)} {

		debug("Creating DB at '",filename,"'");

		if(fd < 0) {
			throw std::system_error(errno,std::system_category(),filename);
		}

		off_t length = ::lseek(fd,0L,SEEK_END);
		if(length == (off_t) -1) {
			throw std::system_error(errno,std::system_category(),"Cant get DB file length");
		}

		if(length == 0) {
			const char *pname = PACKAGE_NAME;
			write(pname,strlen(pname)+1);
		}


	}

	MemCachedDB::DataFile::~DataFile() {

		lock_guard<recursive_mutex> lock(guard);

		if(ptr) {
			munmap((void *) ptr,size());
			ptr = nullptr;
		}

		if(fd >= 0) {
			::close(fd);
			fd = -1;
		}
	}

	size_t MemCachedDB::DataFile::size() {
		lock_guard<recursive_mutex> lock(guard);
		off_t length = lseek(fd,0L,SEEK_END);
		if(length == (off_t) -1) {
			throw std::system_error(errno,std::system_category(),"Cant get DB file length");
		}
		return (size_t) length;
	}

	size_t MemCachedDB::DataFile::write(const void *data, size_t length) {
		lock_guard<recursive_mutex> lock(guard);
		if(fd < 0) {
			throw std::logic_error("Unable to write data on closed file");
		}

		off_t offset = lseek(fd,0L,SEEK_END);
		if(offset == (off_t) -1) {
			throw std::system_error(errno,std::system_category(),"Cant get DB file length");
		}

		size_t bytes = length;
		while(bytes > 0) {
			ssize_t w = ::write(fd, data, bytes);
			if(w < 0) {
				throw std::system_error(errno,std::system_category(),"Error writing to DB file");
			}
			bytes -= w;
			data = (void *) ( ((uint8_t *) data) + w );
		}

		return (size_t) offset;
	}

	void MemCachedDB::DataFile::read(size_t offset, void *data, size_t length) {
		lock_guard<recursive_mutex> lock(guard);
		if(fd < 0) {
			throw std::logic_error("Unable to write data on closed file");
		}

		size_t bytes = length;
		while(bytes > 0) {
			ssize_t r = pread(fd,data,length,offset);
			if(r < 0) {
				throw std::system_error(errno,std::system_category(),"Error reading from DB file");
			} else if(r == 0) {
				throw std::logic_error("Unexpected EOF reading from DB file");
			}
			bytes -= r;
			data = (void *) ( ((uint8_t *) data) + r );

		}

	}

 }

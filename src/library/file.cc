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
  * @brief Implement raw file.
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/memdb/file.h>
 #include <udjat/tools/logger.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <stdexcept>
 #include <sys/mman.h>

 //#ifdef HAVE_UNISTD_H
	#include <unistd.h>
 //#endif // HAVE_UNISTD_H

 #include <sys/mman.h>
 #include <cstdint>

 using namespace std;

 namespace Udjat {

	MemCachedDB::File::File() : fd{open("/tmp",O_RDWR|O_CREAT|O_TRUNC|O_TMPFILE,0600)} {
		debug("Creating temporary file");
		if(fd < 0) {
			throw std::system_error(errno,std::system_category(),"Unable to create temporary file");
		}
	}

	MemCachedDB::File::File(const char *filename) : fd{::open(filename,O_RDWR|O_CREAT,0640)} {

		debug("Creating DB at '",filename,"'");

		if(fd < 0) {
			throw std::system_error(errno,std::system_category(),filename);
		}
	}

	MemCachedDB::File::~File() {

		if(ptr) {
			munmap((void *) ptr,size());
			ptr = nullptr;
		}

		if(fd >= 0) {
			::close(fd);
			fd = -1;
		}
	}

	size_t MemCachedDB::File::size() {
		off_t length = lseek(fd,0L,SEEK_END);
		if(length == (off_t) -1) {
			throw std::system_error(errno,std::system_category(),"Cant get DB file length");
		}
		return (size_t) length;
	}

	size_t MemCachedDB::File::write(const void *data, size_t length) {

		if(fd < 0) {
			throw std::logic_error("Unable to write data on closed file");
		}

		if(ptr) {
			throw std::logic_error("The data file is already mapped and read-only");
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

	size_t MemCachedDB::File::write(const char *data) {
		return write((void *) data, strlen(data)+1);
	}

	void MemCachedDB::File::read(size_t offset, void *data, size_t length) {

		if(fd < 0) {
			throw std::logic_error("Unable to write data on closed file");
		}

		size_t bytes = length;
		while(bytes > 0) {

#ifdef _WIN32
			if(lseek(fd,offset,SEEK_SET) == offset) {
				throw std::system_error(errno,std::system_category(),"Cant setup DB file position");
			}
			ssize_t r = read(fd,data,length);
#else
			ssize_t r = pread(fd,data,length,offset);
#endif // _WIN32

			if(r < 0) {
				throw std::system_error(errno,std::system_category(),"Error reading from DB file");
			} else if(r == 0) {
				throw std::logic_error("Unexpected EOF reading from DB file");
			}

			bytes -= r;
			offset += r;
			data = (void *) ( ((uint8_t *) data) + r );

		}

	}

 }

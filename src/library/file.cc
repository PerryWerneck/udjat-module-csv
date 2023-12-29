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
 #include <udjat/tools/datastore/file.h>
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

	DataStore::File::File() : fd{open("/tmp",O_RDWR|O_CREAT|O_TRUNC|O_TMPFILE,0600)} {
		debug("Creating temporary file");
		if(fd < 0) {
			throw std::system_error(errno,std::system_category(),"Unable to create temporary file");
		}
	}

	DataStore::File::File(const char *filename) : fd{::open(filename,O_RDWR|O_CREAT,0640)} {

		debug("Creating DB at '",filename,"'");

		if(fd < 0) {
			throw std::system_error(errno,std::system_category(),filename);
		}
	}

	DataStore::File::~File() {

		unmap();

		std::lock_guard<std::mutex> lock(guard);
		if(fd >= 0) {
			::close(fd);
			fd = -1;
		}
	}

	size_t DataStore::File::size() {
		off_t length = lseek(fd,0L,SEEK_END);
		if(length == (off_t) -1) {
			throw std::system_error(errno,std::system_category(),"Cant get DB file length");
		}
		return (size_t) length;
	}

	const void * DataStore::File::get_void_ptr(size_t offset) const {

		if(ptr != nullptr) {
			return (ptr+offset);
		}

		throw logic_error("File is not mapped");
	}

	void DataStore::File::map() {

		std::lock_guard<std::mutex> lock(guard);

		if(fd < 0) {
			throw std::logic_error("Unable to map closed file");
		}

		ptr = (uint8_t *) mmap(NULL, size(), PROT_READ, MAP_SHARED, fd, 0);
		if(ptr == MAP_FAILED) {
			ptr = nullptr;
			throw std::system_error(errno,std::system_category(),"Unable to map data file");
		}

	}

	void DataStore::File::unmap() {

		std::lock_guard<std::mutex> lock(guard);

		if(ptr) {
			if(munmap((void *) ptr,size())) {
				throw std::system_error(errno,std::system_category(),"Unable to unmap data file");
			}
			ptr = nullptr;
		}
	}

	size_t DataStore::File::write(const void *data, size_t length) {

		std::lock_guard<std::mutex> lock(guard);

		if(fd < 0) {
			throw std::logic_error("Unable to write data on closed file");
		}

		if(ptr) {
			throw std::logic_error("Unable to write data on mapped file");
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

	void DataStore::File::write(size_t offset, const void *data, size_t length) {

		std::lock_guard<std::mutex> lock(guard);

		if(fd < 0) {
			throw std::logic_error("Unable to write data on closed file");
		}

		if(ptr) {
			throw std::logic_error("Unable to write data on mapped file");
		}

		size_t bytes = length;
		while(bytes > 0) {
			ssize_t w = ::pwrite(fd, data, bytes, offset);
			if(w < 0) {
				throw std::system_error(errno,std::system_category(),"Error writing to DB file");
			}
			bytes -= w;
			offset += w;
			data = (void *) ( ((uint8_t *) data) + w );
		}

	}

	size_t DataStore::File::write(const char *data) {
		return write((void *) data, strlen(data)+1);
	}

	std::string DataStore::File::read(size_t offset) {

		if(ptr) {
			return string{(char *) (ptr+offset)};
		}

		// Not mapped, read from file.
		std::lock_guard<std::mutex> lock(guard);

		if(fd < 0) {
			throw std::logic_error("Unable to read from closed file");
		}

		string text;

		while(1) {

			char buffer[128];

#ifdef _WIN32
			if(lseek(fd,offset,SEEK_SET) == offset) {
				throw std::system_error(errno,std::system_category(),"Cant setup DB file position");
			}
			ssize_t r = read(fd,buffer,128);
#else
			ssize_t r = pread(fd,buffer,128,offset);
#endif // _WIN32

			if(r < 0) {
				throw std::system_error(errno,std::system_category(),"Error reading from DB file");
			} else if(r == 0) {
				throw std::logic_error("Unexpected EOF reading from DB file");
			}

			if(memchr(buffer,0,128)) {
				text.append(buffer);
				break;
			} else {
				text.append(buffer,128);
			}

			offset += r;
		}

		return text;
	}

	void DataStore::File::read(size_t offset, void *data, size_t length) {

		if(ptr) {
			memcpy(data,ptr+offset,length);
			return;
		}

		// Not mapped, read from file.
		std::lock_guard<std::mutex> lock(guard);

		if(fd < 0) {
			throw std::logic_error("Unable to read from closed file");
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

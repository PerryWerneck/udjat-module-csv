#
# spec file for package udjat-module-csv
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

%define product_name %(pkg-config --variable=product_name libudjat)
%define module_path %(pkg-config --variable=module_path libudjat)

Summary:		CSV parser for %{product_name}
Name:			udjat-module-csv
Version:		1.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/udjat-module-csv

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:	gcc-c++

BuildRequires:	pkgconfig(libudjat)

%description
CSV loader module for %{product_name}

#---[ Library ]-------------------------------------------------------------------------------------------------------

%package -n libudjatcsv%{_libvrs}
Summary:	UDJat CSV load library

%description -n libudjatcsv%{_libvrs}
CSV file parser library for udjat

#---[ Development ]---------------------------------------------------------------------------------------------------

%package -n udjat-csv-devel
Summary:	Development files for %{name}
Requires:	pkgconfig(libudjat)
Requires:	pkgconfig(dbus-1)
Requires:	libudjatcsv%{_libvrs} = %{version}

%description -n udjat-csv-devel

Development files for %{product_name}'s csv parser library.

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup

NOCONFIGURE=1 \
	./autogen.sh

%configure 

%build
make all

%install
%makeinstall

%files
%defattr(-,root,root)
%{module_path}/*.so

%files -n libudjatcsv%{_libvrs}
%defattr(-,root,root)
%{_libdir}/libudjatcsv.so.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files -n udjat-csv-devel
%defattr(-,root,root)
%dir %{_includedir}/udjat/tools/datastore
%dir %{_includedir}/udjat/tools/datastore/columns
%{_includedir}/udjat/tools/datastore/*.h
%{_includedir}/udjat/tools/datastore/columns/*.h
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%pre -n libudjatcsv%{_libvrs} -p /sbin/ldconfig

%post -n libudjatcsv%{_libvrs} -p /sbin/ldconfig

%postun -n libudjatcsv%{_libvrs} -p /sbin/ldconfig

%changelog


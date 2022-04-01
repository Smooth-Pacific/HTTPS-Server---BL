#!/bin/bash


# Runs configurations after library is cloned
dir=httplib/libhttpserver

if [[ ! -e $dir ]]; then
	cd httplib
    git clone https://github.com/etr/libhttpserver.git
    cd libhttpserver
    bash ./bootstrap
    mkdir build
    cd build
else
	cd httplib/libhttpserver/build
fi

bash ../configure CPPFLAGS=-I/usr/local/include LDFLAGS=-L/usr/local/lib
make
make install


# configs for sql3
# cd /home/sqlite
# ./configure --prefix=/usr/local
# make
# make install

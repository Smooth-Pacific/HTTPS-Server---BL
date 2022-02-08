#!/bin/bash

cd httplib/libhttpserver/build
bash ../configure CPPFLAGS=-I/usr/local/include LDFLAGS=-L/usr/local/lib
make
make install


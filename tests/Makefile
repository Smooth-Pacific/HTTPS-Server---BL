CC = clang++
SHORTFLAGS = -std=c++17 -pthreads -fopenmp -ltbb -g -W -Wall -lsqlite3 -lxml2 $(shell pkg-config --cflags xmlsec1)
INCLUDE = -Iinclude -I/home/include -I/usr/local/include -I/usr/include/libxml2 -I/usr/include/xmlsec1
LDFLAGS = -L/usr/local/lib $(shell pkg-config --libs xmlsec1)

TARGET = test
OTHER = ../server/DSign.cpp ../server/dataGen.cpp ../server/dataProcess.cpp

.PHONY: all clean


all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(INCLUDE) $(LDFLAGS) $(SHORTFLAGS) $(LIBRARIES) -o tests $(TARGET).cpp $(OTHER) 


clean:
	rm -f *.o
	rm -f files/*.xml
	rm -f tests

CC = clang++
CFLAGS = -std=c++17 -pthreads -Wall -g
INCLUDE = -Iinclude -I/usr/local/include 
LDFLAGS = -L/usr/local/lib
LIBRARIES = -L. -lhttpserver

TARGET = main
OTHER = startConfig.cpp logging.cpp

.PHONY: all clean


all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(INCLUDE) $(LDFLAGS) $(CFLAGS) $(LIBRARIES) -o server $(TARGET).cpp $(OTHER) 

curl:
	curl -v -XGET --digest --user myuser:mypass 'https://localhost:8080'
six:
	curl -v -XGET --digest --user myuser:mypass 'https://\[::1\]:8080'
xml:
	curl -v -XGET --digest --user myuser:mypass -O 'https://localhost:8080/mime/xml'
post:
	curl -v -XPOST -H "Content-Type:text/xml" -H "Accept: application/xml" -d @files/sample.xml --digest --user myuser:mypass 'https://localhost:8080'

clean:
	rm -f *.o
	rm -f server
	rm -f log.txt
	rm -rf apmlogs
	#rm -rf data_throughput
	rm -f data_throughput/write*
	rm -f data_throughput/read*

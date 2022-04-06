CC = clang++
CFLAGS = -std=c++17 -pthreads -g -W -Wall $(shell pkg-config --cflags xmlsec1)
SHORTFLAGS = -std=c++17 -pthreads -fopenmp -ltbb -g -W -Wall -lsqlite3
INCLUDE = -Iinclude -I/home/include -I/usr/local/include -I/usr/include/libxml2 -I/usr/include/xmlsec1
LDFLAGS = -L/usr/local/lib
LIBRARIES = -L. -lhttpserver -lxml2 $(shell pkg-config --libs xmlsec1) -lsqlite3

TARGET = main
OTHER = startConfig.cpp logging.cpp DSign.cpp
DATAGEN = dataGen
DATAPROCESS = dataProcess

.PHONY: all clean


all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(INCLUDE) $(LDFLAGS) $(CFLAGS) $(LIBRARIES) -o server $(TARGET).cpp $(OTHER) 

data: $(DATAGEN)
	
$(DATAGEN): $(DATAGEN).cpp
	$(CC) $(INCLUDE) $(LDFLAGS) $(SHORTFLAGS) -lxml2 -o exec $(DATAGEN).cpp 

process: $(DATAPROCESS)
	
$(DATAPROCESS): $(DATAPROCESS).cpp
	$(CC) $(INCLUDE) $(LDFLAGS) $(SHORTFLAGS) -lxml2 -o proc $(DATAPROCESS).cpp $(DATAGEN).cpp 



curl:
	curl -v -XGET --digest --user myuser:mypass 'https://localhost:8080'
six:
	curl -v -XGET --digest --user myuser:mypass 'https://\[::1\]:8080'
xml:
	curl -v -XGET --digest --user myuser:mypass -O 'https://localhost:8080/mime/xml'
user:
	curl -v -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/user'
signed:
	curl -v -XGET --digest --user myuser:mypass -O 'https://localhost:8080/transaction/process/1/percentageFraudByYear'
post:
	curl -v -XPOST -H "Content-Type:text/xml" -H "Accept: application/xml" -d @files/sample.xml --digest --user myuser:mypass 'https://localhost:8080/mime/xml'

clean:
	rm -f *.o
	rm -f server
	rm -f exec
	rm -f proc
	rm -f log.txt
	rm -rf apmlogs
	rm -rf data_throughput

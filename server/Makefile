CC = clang++
CFLAGS = -std=c++17 -pthreads -Wall -g
INCLUDE = -Iinclude -I/usr/local/include 
LDFLAGS = -L/usr/local/lib
LIBRARIES = -L. -lhttpserver

TARGET = main
CONF = startConfig

.PHONY: all clean


all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(INCLUDE) $(LDFLAGS) $(CFLAGS) $(LIBRARIES) -o server $(TARGET).cpp $(CONF).cpp
	
clean:
	rm -f *.o

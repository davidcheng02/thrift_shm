CC=g++

CFLAGS=--std=c++11

THRIFT_FILES=gen-cpp/echo_types.cpp gen-cpp/HelloSvc.cpp

all: client server

server: server.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $? -o server -lthrift

client: client.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $? -o client -lthrift

clean:
	rm client server

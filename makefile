CC=g++

CFLAGS=--std=c++11

THRIFT_FILES=gen-cpp/echo_types.cpp gen-cpp/HelloSvc.cpp

all: client server test

server: server.cpp buffer.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o server -lthrift

client: client.cpp buffer.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o client -lthrift

test: test.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o test -lthrift

clean:
	rm client server test

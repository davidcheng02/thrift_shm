CC=g++

CFLAGS=--std=c++11

THRIFT_FILES=gen-cpp/echo_types.cpp gen-cpp/HelloSvc.cpp

all: client server test buffer

server: server.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $? -o server -lthrift

client: client.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $? -o client -lthrift

test: test.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $? -o test -lthrift

buffer: buffer.cpp
	$(CC) $(CFLAGS) $?

clean:
	rm client server test buffer

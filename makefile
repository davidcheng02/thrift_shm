CC=g++

CFLAGS=--std=c++11

THRIFT_FILES= gen-cpp/HelloSvc.cpp

all: client server test http_server http_client shm_lib_server shm_lib_client

http: http_server http_client

shm_lib: shm_lib_server shm_lib_client

server: server.cpp buffer.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o server -lthrift

http_server: http_server.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o http_server -lthrift

shm_lib_server: shm_lib_server.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o shm_lib_server -lthrift

client: client.cpp buffer.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o client -lthrift

http_client: http_client.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o http_client -lthrift

shm_lib_client: shm_lib_client.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o shm_lib_client -lthrift

test: test.cpp
	$(CC) $(CFLAGS) $(THRIFT_FILES) $^ -o test -lthrift

clean:
	rm client server test http_server http_client shm_lib_server shm_lib_client

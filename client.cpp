#include "buffer.h"
#include "gen-cpp/HelloSvc.h"
#include "gen-cpp/echo_types.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

int main() {
    try {
        Buffer buf = Buffer(true);

        std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(SHMSZ));
        std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        transport->open();

        // Debugging using user input
        std::string input;
        std::cout << "Type a message: ";
        std::cin >> input;

        protocol->writeString(input);

        // buffer pointer and data size
        uint8_t *pbuf;
        uint32_t pbuf_sz;
        transport->getBuffer(&pbuf, &pbuf_sz);

        char *shm = buf.putShm();

        // CLIENT WORK
        // copy serialized data into shared memory
        memcpy(shm, pbuf, SHMSZ);
        std::cout << "Copied serialized data to shmem" << std::endl;

        buf.putShmRelease();

        buf.detachShm();
    } catch (char const *e) {
        std::cout << e << std::endl;
        return 1;
    }

    return 0;
}

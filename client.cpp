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
        Buffer request_buf = Buffer(true, 1, 1);
        Buffer response_buf = Buffer(false, 2, 2);

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
        transport->resetBuffer();

        char *shm = request_buf.putShm();

        // CLIENT WORK
        // copy serialized data into shared memory
        memcpy(shm, pbuf, SHMSZ);
        std::cout << "Copied serialized data to shmem" << std::endl;

        request_buf.putShmRelease();
        request_buf.detachShm();

        // get response from server and output
        shm = response_buf.processShm();
        pbuf = (uint8_t *) malloc(sizeof *pbuf * SHMSZ);

        memcpy(pbuf, shm, SHMSZ);
        transport->write(pbuf, SHMSZ);
        std::string response;
        protocol->readString(response);
        std::cout << "Response from server: " << response << std::endl;

        free(pbuf);
        response_buf.processShmRelease();
        response_buf.freeShm();
    } catch (char const *e) {
        std::cout << e << std::endl;
        return 1;
    }

    return 0;
}

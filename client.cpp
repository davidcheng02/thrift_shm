#include "buffer.h"
#include "gen-cpp/HelloSvc.h"
#include "gen-cpp/echo_types.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

#define NUMRUNS 100000

void echoMsg(Buffer request_buf,
             Buffer response_buf,
             std::shared_ptr<TMemoryBuffer> transport,
             std::shared_ptr<TProtocol> protocol,
             uint8_t *pbuf,
             std::string msg) {
    protocol->writeString(msg);

    // buffer pointer and data size
    uint8_t *transport_buf;
    uint32_t transport_buf_sz;
    transport->getBuffer(&transport_buf, &transport_buf_sz);
    transport->resetBuffer();

    char *shm = request_buf.putShm();

    // CLIENT WORK
    // copy serialized data into shared memory
    memcpy(shm, transport_buf, SHMSZ);

    request_buf.putShmRelease();

    // get response from server and output
    shm = response_buf.processShm();

    memcpy(pbuf, shm, SHMSZ);
    transport->write(pbuf, SHMSZ);
    std::string response;
    protocol->readString(response);

    assert(response == "Hello, " + msg);
    transport->resetBuffer();

    response_buf.processShmRelease();
}

int main() {
    std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(SHMSZ));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    uint8_t *pbuf = (uint8_t *) malloc(sizeof *pbuf * SHMSZ);
    transport->open();

    try {
        Buffer request_buf = Buffer(true, 1, 1);
        Buffer response_buf = Buffer(false, 2, 2);

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < NUMRUNS; ++i) {
            echoMsg(request_buf,
                    response_buf,
                    transport,
                    protocol,
                    pbuf,
                    "World");
        }

        auto stop = std::chrono::high_resolution_clock::now();

        typedef std::chrono::milliseconds ms;
        typedef std::chrono::microseconds us;
        typedef std::chrono::duration<float> fsec;

        fsec duration = stop - start;

        double throughput = NUMRUNS / duration.count();
        us duration_us = std::chrono::duration_cast<us>(duration);
        double latency = duration_us.count() / NUMRUNS;

        std::cout << "Throughput: " << throughput << " req/s" << std::endl;
        std::cout << "Latency: " << latency << " us" << std::endl;
        std::cout << "Time: " << duration.count() << " s" << std::endl;

        // send shutdown msg to server
        echoMsg(request_buf, response_buf, transport, protocol, pbuf, "*");

        // shut down stuff, client responsible for freeing response_buf
        free(pbuf);
        request_buf.detachShm();
        response_buf.freeShm();
    } catch (char const *e) {
        std::cout << e << std::endl;
        return 1;
    }

    return 0;
}

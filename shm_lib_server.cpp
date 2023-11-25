#include "notnets_shm/src/rpc.h"

#include "gen-cpp/HelloSvc.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <iostream>
#include <signal.h>
#include <sstream>

#define NUMRUNS 100000

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

void sendResponse(std::shared_ptr<TMemoryBuffer> transport,
                  std::shared_ptr<TProtocol> protocol,
                  uint8_t* pbuf,
                  int message_size,
                  queue_pair* qp,
                  std::string response,
                  bool serialize) {

    if (serialize) {
        protocol->writeString(response);

        // buffer pointer and data size
        uint8_t *transport_buf;
        uint32_t transport_buf_sz;
        transport->getBuffer(&transport_buf, &transport_buf_sz);
        transport->resetBuffer();
        assert(transport_buf_sz <= message_size);

        server_send_rpc(qp, transport_buf, message_size);
    } else {
        server_send_rpc(qp, response.c_str(), message_size);
    }

}

std::string receiveRequest(std::shared_ptr<TMemoryBuffer> transport,
                           std::shared_ptr<TProtocol> protocol,
                           uint8_t* pbuf,
                           int message_size,
                           queue_pair* qp,
                           bool serialize) {
    std::string request;
    server_receive_buf(qp, pbuf, message_size);

    if (serialize) {
        transport->write(pbuf, message_size);
        protocol->readString(request);
        transport->resetBuffer();
    } else {
        request = (char*) pbuf;
    }

    return request;
}

int main(int argc, char **argv) {
    bool serialize = true;

    if (argc > 1 && strcmp(argv[1], "--no-serialize")) {
        serialize = false;
    }

    int message_size = sizeof(char)*20;
    std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(message_size));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    uint8_t *pbuf = (uint8_t *) malloc(sizeof *pbuf * message_size);

    try {
        server_context* sc = register_server((char*) "test_server_addr");

        queue_pair* qp;
        while ((qp = accept(sc)) == NULL);

        // uncomment if you want server to handle requests indefinitely. need
        // to uncomment break string code below as well.
//        while (true) {
        for (int i = 0; i < NUMRUNS; ++i) {
            std::string request = receiveRequest(transport,
                                                 protocol,
                                                 pbuf,
                                                 message_size,
                                                 qp,
                                                 serialize);

            // break string
//            if (request == "*") {
//                break;
//            }

            // send response
            // do work on request
            std::string response = "Hello, " + request;
            sendResponse(transport,
                         protocol,
                         pbuf,
                         message_size,
                         qp,
                         response,
                         serialize);
        }

        free(pbuf);
        shutdown(sc);
    } catch (char const *e) {
        std::cout << e << std::endl;
    }

    return 0;
}


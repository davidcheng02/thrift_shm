#include "notnets_shm/src/rpc.h"

#include "gen-cpp/HelloSvc.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <chrono>
#include <iostream>
#include <signal.h>
#include <sstream>

#define NUMRUNS 100000

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

void sendRequest(std::shared_ptr<TMemoryBuffer> transport,
                 std::shared_ptr<TProtocol> protocol,
                 uint8_t* pbuf,
                 int message_size,
                 queue_pair* qp,
                 std::string request,
                 bool serialize) {
    if (serialize) {
        protocol->writeString(request);

        // buffer pointer and data size
        uint8_t *transport_buf;
        uint32_t transport_buf_sz;
        transport->getBuffer(&transport_buf, &transport_buf_sz);
        transport->resetBuffer();

        assert(transport_buf_sz <= message_size);

        client_send_rpc(qp, transport_buf, message_size);
    } else {
        client_send_rpc(qp, request.c_str(), message_size);
    }
}

std::string receiveResponse(std::shared_ptr<TMemoryBuffer> transport,
                            std::shared_ptr<TProtocol> protocol,
                            uint8_t* pbuf,
                            int message_size,
                            queue_pair* qp,
                            bool serialize) {
    std::string response;
    client_receive_buf(qp, pbuf, message_size);

    if (serialize) {
        transport->write(pbuf, message_size);
        protocol->readString(response);
        transport->resetBuffer();
    } else {
        response = (char*) pbuf;
    }

    return response;
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
    transport->open();

    try {
        queue_pair* qp = client_open((char*) "test_client_addr",
                                     (char*) "test_server_addr",
                                     message_size);

        while (qp == NULL) {
            qp = client_open((char*) "test_client_addr",
                             (char*) "test_server_addr",
                             message_size);
        }

        spsc_queue_header* header = get_queue_header(qp->request_shmaddr);
        int batch_size = header->queue_size;
        int total_iter = 0;

        auto start = std::chrono::high_resolution_clock::now();

        while (total_iter < NUMRUNS) {
            int iter = 0;
            for (; iter < batch_size && total_iter < NUMRUNS;
                    ++iter, ++total_iter) {
                sendRequest(transport,
                            protocol,
                            pbuf,
                            message_size,
                            qp,
                            "World",
                            serialize);
            }

            for (int i = 0; i < iter; ++i) {
                std::string response = receiveResponse(transport,
                                               protocol,
                                               pbuf,
                                               message_size,
                                               qp,
                                               serialize);
                assert(response == "Hello, World");
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();

        typedef std::chrono::milliseconds ms;
        typedef std::chrono::microseconds us;
        typedef std::chrono::duration<float> fsec;

        fsec duration = stop - start;

        double throughput = NUMRUNS / duration.count();
        us duration_us = std::chrono::duration_cast<us>(duration);
        double latency = (double) duration_us.count() / NUMRUNS;

        std::cout << "Throughput: " << throughput << " req/s" << std::endl;
        std::cout << "Latency: " << latency << " us" << std::endl;
        std::cout << "Time: " << duration.count() << " s" << std::endl;

        // send close signal
//        sendRequest(transport,
//                    protocol,
//                    pbuf,
//                    message_size,
//                    qp,
//                    "*",
//                    serialize);

        client_close((char*) "test_client_addr", (char*) "test_server_addr");
        free(pbuf);
    } catch (const char * e) {
        std::cout << e << std::endl;
    }

    return 0;
}

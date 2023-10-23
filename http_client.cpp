#include "gen-cpp/HelloSvc.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <cassert>
#include <chrono>
#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

#define NUMRUNS 100000

int main() {
    std::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HelloSvcClient client(protocol);

    try {
        transport->open();

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < NUMRUNS; ++i) {
            std::string msg = "World";
            std::string response;

            client.hello(response, msg);

            assert(response == "Hello, World");
//            std::cout << "Server response: " << response << std::endl;
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
    } catch(TException& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }

    return 0;
}

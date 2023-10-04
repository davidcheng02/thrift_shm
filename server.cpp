// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "gen-cpp/HelloSvc.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <iostream>
#include <sys/shm.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class HelloSvcHandler : virtual public HelloSvcIf {
public:
    HelloSvcHandler() {
        std::cout << "Starting server..." << std::endl;
    }

    void hello(std::string& _return, const std::string& msg) {
        _return = "Hello, " + msg + "\n";
    }
};

int main(int argc, char **argv) {
//    int port = 9090;
//    std::shared_ptr<HelloSvcHandler> handler(new HelloSvcHandler());
//    std::shared_ptr<TProcessor> processor(new HelloSvcProcessor(handler));
//    std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
//    std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
//    std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
//
//    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
//    server.serve();
    uint32_t sz = 1024;
    char *shm;

    std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(sz));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    uint8_t *pbuf = (uint8_t *) malloc(sizeof *pbuf * sz);
    Message *msg = new Message();

    key_t key = 5678;
    int shmid;

    if ((shmid = shmget(key, sz, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = (char *) shmat(shmid, 0, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    memcpy(pbuf, shm, sz);
    std::cout << "Copied shmem to pbuf" << std::endl;
    transport->write(pbuf, sz);
    msg->read(protocol.get());
    msg->printTo(std::cout);
    std::cout << std::endl;

    free(pbuf);

    if ((shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1) {
        perror("shmctl");
        exit(1);
    }

    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    std::cout << "Detached from shmem" << std::endl;

    return 0;
}

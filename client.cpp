#include "gen-cpp/HelloSvc.h"
#include "gen-cpp/echo_types.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>

#include <chrono>
#include <iostream>
#include <sys/shm.h>
#include <thread>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

int main() {
    uint32_t sz = 1024;
    char *shm;

    std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(sz));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    Message *msg = new Message();
    msg->msg = "World";

    // write to protocol, which will serialize it and send to transport
    msg->write(protocol.get());

    key_t key = 5678;
    // allocates shmem block, returns key/id of block
    int shmid;
    if ((shmid = shmget(key, sz, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    std::cout << shmid << std::endl;

    // buffer pointer and data size
    uint8_t *pbuf;
    transport->getBuffer(&pbuf, &sz);

    if ((shm = (char *) shmat(shmid, 0, 0)) == (char *) -1) {
        perror("shmat");
    }

    // copy serialized data into shared memory
    memcpy(shm, pbuf, sz);
    std::cout << "Copied serialized data to shmem" << std::endl;

//    if ((shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1) {
//        perror("shmctl");
//        exit(1);
//    }
//
//    std::this_thread::sleep_for(std::chrono::seconds(50));
//
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    std::cout << "Detached from shmem" << std::endl;


//    HelloSvcClient client(protocol);
//
//    try {
//        transport->open();
//
//        std::cout << "Initializing contact with server" << std::endl;
//        std::string res;
//        client.hello(res, "David");
//        std::cout << "Returned message: " << res << std::endl;
//    } catch (TException &tx) {
//        std::cout << "Error: " << tx.what() << std::endl;
//    }

    return 0;
}

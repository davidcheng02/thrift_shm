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
    uint32_t sz = 1024;

    std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(sz));
    std::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));

    std::string msg = "World";
    protocol->writeString(msg);

    uint8_t *pbuf;
    uint32_t pbuf_sz;

    transport->getBuffer(&pbuf, &pbuf_sz);
    std::cout << "pbuf: " << std::endl;
    for (int i = 0; i < pbuf_sz; ++i) {
        std::cout << "char: " << int(*(pbuf + i)) << std::endl;
    }

    int32_t size;
    protocol->readI32(size);
    std::cout << size << std::endl;
}


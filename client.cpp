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

#define SEMKEYPATH "/dev/null"
#define SEMKEYID 1

#define NUMSEMS 2

int main() {
    // SEMAPHORE INITIALIZATION
    struct sembuf operations[2];
    int semid, rc;
    key_t semkey;

    semkey = ftok(SEMKEYPATH, SEMKEYID);
    if (semkey == (key_t) -1) {
        std::cout << "main: ftok() for sem failed" << std::endl;
        return -1;
    }

    semid = semget(semkey, NUMSEMS, 0666);
    if (semid == -1) {
        std::cout << "main: semget() failed" << std::endl;
        return -1;
    }
    // ======
    uint32_t sz = 10;
    char *shm;

    std::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer(sz));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    transport->open();

    // Debugging using user input
    std::string input;
    std::cout << "Type a message: ";
    std::cin >> input;

    protocol->writeString(input);

    key_t key = 5678;
    // get shmem block, which should already be allocated by server
    int shmid;
    if ((shmid = shmget(key, sz, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // buffer pointer and data size
    uint8_t *pbuf;
    uint32_t pbuf_sz;
    transport->getBuffer(&pbuf, &pbuf_sz);
    std::cout << "pbuf sz: " << pbuf_sz << std::endl;
    std::cout << "pbuf: " << std::endl;
    for (int i = 0; i < pbuf_sz; ++i) {
        std::cout << "char: " << *(pbuf + i) << std::endl;
    }

    if ((shm = (char *) shmat(shmid, 0, 0)) == (char *) -1) {
        perror("shmat");
    }

    // check if 1st semaphore is 0. if not, then it will wait until sem is 0
    operations[0].sem_num = 0;
    operations[0].sem_op = 0;
    operations[0].sem_flg = 0;

    // increment 1st sem to indicate that it is busy
    operations[1].sem_num = 0;
    operations[1].sem_op = 1;
    operations[1].sem_flg = 0;

    rc = semop(semid, operations, 2);
    if (rc == -1) {
        std::cout << "main: semop() failed" << std::endl;
    }

    // CLIENT WORK
    // copy serialized data into shared memory
    memcpy(shm, pbuf, sz);
    std::cout << "Copied serialized data to shmem" << std::endl;

    // decrement 1st sem, indicating shmem is not in use
    operations[0].sem_num = 0;
    operations[0].sem_op = -1;
    operations[0].sem_flg = 0;

    // increment 2nd sem, indicating client has added to shmem, so server
    // can process it
    operations[1].sem_num = 1;
    operations[1].sem_op = 1;
    operations[1].sem_flg = 0;

    rc = semop(semid, operations, 2);
    if (rc == -1) {
        std::cout << "main: semop() failed" << std::endl;
    }

    // cleanup, detach shmem from this client
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    std::cout << "Detached from shmem" << std::endl;

    return 0;
}

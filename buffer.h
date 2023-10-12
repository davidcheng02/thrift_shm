#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMKEYPATH "/dev/null"
#define SEMKEYID 1
#define NUMSEMS 2

#define SHMKEY 5678
#define SHMSZ 1024

class Buffer {
    private:
        int semid, rc;
        key_t semkey;
        int shmid;
        struct sembuf operations[2];
        short sarray[NUMSEMS];

        char *shm;
    public:
        Buffer(bool exists);
        char *processShm();
        void processShmRelease();
        char *putShm();
        void putShmRelease();
        void detachShm();
        void freeShm();
};

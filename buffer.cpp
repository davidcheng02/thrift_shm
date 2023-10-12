#include "buffer.h"

Buffer::Buffer(bool exists) {
    // set up NUMSEM semaphores
    semkey = ftok(SEMKEYPATH, SEMKEYID);
    if (semkey == (key_t) -1) {
        throw "ftok() for sem failed";
    }

    std::cout << "Sem key: " << std::hex << semkey << std::endl;

    if (exists) {
        semid = semget(semkey, NUMSEMS, 0666);
    } else {
        semid = semget(semkey, NUMSEMS, 0666 | IPC_CREAT | IPC_EXCL);
    }

    if (semid == -1) {
        throw "semget() failed";
    }

    std::cout << "Sem id: " << semid << std::endl;

    // first semaphore definitions:
    //     1 = shmem being used
    //     0 = shmem is free
    // second semaphore definitions:
    //     1 = shmem changed by client
    //     0 = shmem not changed by client
    sarray[0] = 0;
    sarray[1] = 0;

    rc = semctl(semid, 1, SETALL, sarray);
    if (rc == -1) {
        throw "semctl() initialization failed";
    }

    // set up shmem
    if (exists) {
        shmid = shmget(SHMKEY, SHMSZ, 0666);
    } else {
        shmid = shmget(SHMKEY, SHMSZ, 0666 | IPC_CREAT | IPC_EXCL);
    }

    if (shmid < 0) {
        throw "shmget() failed";
    }

    std::cout << "Shm id: " << shmid << std::endl;

    if ((shm = (char *) shmat(shmid, 0, 0)) == (char *) -1) {
        throw "shmat() failed";
    }
}

// get ownership of shm, so other apps can't access shm
char *Buffer::processShm() {
    // operate on 2nd sem
    operations[0].sem_num = 1;
    // decrement semval by 1, implying that client msg has been processed,
    // so client can continue to send msgs
    operations[0].sem_op = -1;
    // allow for wait
    operations[0].sem_flg = 0;

    // operate on 1st sem
    operations[1].sem_num = 0;
    // increment semval by 1, implying that shmem being used (by server)
    operations[1].sem_op = 1;
    // don't allow for wait, and i believe waiting unnecessary because
    // server is not writing to shmem
    operations[1].sem_flg = IPC_NOWAIT;

    rc = semop(semid, operations, 2);
    if (rc == -1) {
        throw "semop() failed";
    }
    return shm;
}

void Buffer::processShmRelease() {
    // signal first sem to free shmem
    operations[0].sem_num = 0;
    operations[0].sem_op = -1;
    operations[0].sem_flg = IPC_NOWAIT;

    rc = semop(semid, operations, 1);
    if (rc == -1) {
        throw "semop() failed";
    }
}

// going to put something into shm buffer
// returns shm buffer so client can put put into it
char *Buffer::putShm() {
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
        throw "putShm: semop() failed";
    }

    return shm;
}

// make changes to allow shm to be accessed, and indicate that msg
// has been put onto shm
void Buffer::putShmRelease() {
    // release use of shmem
    operations[0].sem_num = 0;
    operations[0].sem_op = -1;
    operations[0].sem_flg = 0;

    // indicate that msg has been put onto shm
    operations[1].sem_num = 1;
    operations[1].sem_op = 1;
    operations[1].sem_flg = 0;

    rc = semop(semid, operations, 2);
    if (rc == -1) {
        throw "putShmRelease(): semop() failed";
    }
}

void Buffer::detachShm() {
    // detach from this shm
    if (shmdt(shm) == -1) {
        throw "detachShm(): shmdt() failed";
    }

    std::cout << "Detached from shm" << std::endl;
}

void Buffer::freeShm() {
    // free semaphores
    rc = semctl(semid, 1, IPC_RMID);
    if (rc == -1) {
        throw "semctl() remove id failed";
    }

    detachShm();

    if ((shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1) {
        throw "shmctl() failed";
    }
}

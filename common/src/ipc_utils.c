
#include "../inc/ipc_utils.h"
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int create_semaphore(key_t sem_key) {
    int sem_id;
    union semun arg;
    sem_id = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
    
    if (sem_id >= 0) {
        arg.val = 1;
        if (semctl(sem_id, 0, SETVAL, arg) == -1) {
            perror("semctl SETVAL");
            return -1;
        }
    } else if (errno == EEXIST) {
        sem_id = semget(sem_key, 1, 0666);
        if (sem_id == -1) {
            perror("semget existing");
            return -1;
        }
    } else {
        perror("semget create");
        return -1;
    }
    
    return sem_id;
}

int lock_semaphore(int sem_id) {
    struct sembuf sem_op = {0, -1, 0}; 
    
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop lock");
        return -1;
    }
    
    return 0;
}


int unlock_semaphore(int sem_id) {
    struct sembuf sem_op = {0, 1, 0};
    
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop unlock");
        return -1;
    }
    
    return 0;
}

int remove_semaphore(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID, 0) == -1) {
        perror("semctl IPC_RMID");
        return -1;
    }
    
    return 0;
}

int create_shared_memory(key_t shm_key, size_t size) {
    int shm_id;
    
    shm_id = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | 0666);
    
    if (shm_id >= 0) {
        return shm_id;
    } else if (errno == EEXIST) {
        shm_id = shmget(shm_key, size, 0666);
        if (shm_id == -1) {
            perror("shmget existing");
            return -1;
        }
        return shm_id;
    } else {
        perror("shmget create");
        return -1;
    }
}
void *attach_shared_memory(int shm_id) {
    void *ptr;
    
    ptr = shmat(shm_id, NULL, 0);
    if (ptr == (void *)-1) {
        perror("shmat");
        return NULL;
    }
    
    return ptr;
}

int detach_shared_memory(const void *ptr) {
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        return -1;
    }
    
    return 0;
}

int remove_shared_memory(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
        return -1;
    }
    
    return 0;
}

int send_signal(pid_t pid, int sig) {
    if (kill(pid, sig) == -1) {
        perror("kill");
        return -1;
    }
    
    return 0;
}

int setup_signal_handler(int sig, void (*handler)(int)) {
    struct sigaction sa;
    
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(sig, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;
    }
    
    return 0;
}

int setup_alarm_handler(void (*handler)(int), unsigned int interval) {
    if (setup_signal_handler(SIGALRM, handler) == -1) {
        return -1;
    }
    
    alarm(interval);
    
    return 0;
}
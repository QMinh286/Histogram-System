/*
*	FILE:			ipc_utils.c
*	ASSIGNMENT:	The "Histogram System" 
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This file implements the IPC utility functions used in the Histogram System. 
*                 It includes semaphore operations, shared memory management, and signal handling.
*/
#include "../inc/ipc_utils.h"
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

//==================================================FUNCTION========================|
//Name:           create_semaphore                                                     |
//Params:         key_t sem_key        The key for the semaphore to be created.      |
//Returns:        int                   The semaphore ID on success, -1 on failure.   |
//Outputs:        NONE                                                              |
//Description:    This function creates a semaphore with the given key. If the semaphore |
//                already exists, it opens the existing semaphore. If a new semaphore is |
//                created, it initializes it with a value of 1.                       |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           lock_semaphore                                                      |
//Params:         int sem_id           The semaphore ID to be locked.                |
//Returns:        int                   Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function locks the semaphore using the semaphore ID provided. |
//                If the lock operation fails, it returns -1.                        |
//==================================================================================|
int lock_semaphore(int sem_id) {
    struct sembuf sem_op = {0, -1, 0}; 
    
    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("semop lock");
        return -1;
    }
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           unlock_semaphore                                                    |
//Params:         int sem_id           The semaphore ID to be unlocked.              |
//Returns:        int                   Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function unlocks the semaphore using the semaphore ID provided. |
//                If the unlock operation fails, it returns -1.                      |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           create_shared_memory                                                |
//Params:         key_t shm_key         The key for the shared memory segment.       |
//                size_t size           The size of the shared memory segment.       |
//Returns:        int                    The shared memory ID on success, -1 on failure. |
//Outputs:        NONE                                                              |
//Description:    This function creates a shared memory segment with the given key and size. |
//                If the shared memory already exists, it opens the existing segment. |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           attach_shared_memory                                                |
//Params:         int shm_id            The shared memory ID to attach.              |
//Returns:        void*                 A pointer to the attached shared memory, or NULL on failure. |
//Outputs:        NONE                                                              |
//Description:    This function attaches the shared memory segment to the process's address space. |
//                If the attachment fails, it returns NULL.                            |
//==================================================================================|
void *attach_shared_memory(int shm_id) {
    void *ptr;
    
    ptr = shmat(shm_id, NULL, 0);
    if (ptr == (void *)-1) {
        perror("shmat");
        return NULL;
    }
    
    return ptr;
}

//==================================================FUNCTION========================|
//Name:           detach_shared_memory                                               |
//Params:         const void* ptr        A pointer to the shared memory to detach.    |
//Returns:        int                    Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function detaches the shared memory segment from the process's address space. |
//                If the detachment fails, it returns -1.                             |
//==================================================================================|
int detach_shared_memory(const void *ptr) {
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        return -1;
    }
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           remove_shared_memory                                               |
//Params:         int shm_id            The shared memory ID to be removed.           |
//Returns:        int                   Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function removes the shared memory segment using the provided ID. |
//                If the removal fails, it returns -1.                                |
//==================================================================================|
int remove_shared_memory(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
        return -1;
    }
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           send_signal                                                        |
//Params:         pid_t pid             The process ID to send the signal to.        |
//                int sig               The signal to send.                         |
//Returns:        int                    Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function sends a signal to the specified process using the given signal number. |
//                If the signal operation fails, it returns -1.                       |
//==================================================================================|
int send_signal(pid_t pid, int sig) {
    if (kill(pid, sig) == -1) {
        perror("kill");
        return -1;
    }
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           setup_signal_handler                                                |
//Params:         int sig               The signal number to set up a handler for.   |
//                void (*handler)(int)    A function pointer to the signal handler.  |
//Returns:        int                    Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function sets up a signal handler for a specified signal. If it fails to do so, |
//                it returns -1.                                                      |
//==================================================================================|
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


//==================================================FUNCTION========================|
//Name:           setup_alarm_handler                                                 |
//Params:         void (*handler)(int)    A function pointer to the alarm signal handler. |
//                unsigned int interval    The interval (in seconds) between alarms.   |
//Returns:        int                     Returns 0 on success, -1 on failure.         |
//Outputs:        NONE                                                              |
//Description:    This function sets up an alarm signal handler and sets the alarm to trigger at the specified interval. |
//==================================================================================|
int setup_alarm_handler(void (*handler)(int), unsigned int interval) {
    if (setup_signal_handler(SIGALRM, handler) == -1) {
        return -1;
    }
    
    alarm(interval);
    
    return 0;
}
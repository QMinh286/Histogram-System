/*
*	FILE:			dp1.c
*	ASSIGNMENT:	The "Histogram System"
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This file implements the core functionality of the DP1 process. It
*					initializes shared memory and semaphores, launches the DP2 process,
*					generates characters, and writes them to a circular buffer.
*/

#include "../../common/inc/constants.h"
#include "../../common/inc/circular_buffer.h"
#include "../../common/inc/ipc_utils.h"
#include "../inc/dp1.h"

static CircularBuffer *cb = NULL;
static int shm_id = -1;
static int sem_id = -1;
static pid_t dp2_pid = -1;
static int run = 1;

//==================================================FUNCTION========================|
//Name:           dp1_init                                                           |
//Params:         NONE                                                              |
//Returns:        int                     0 on success, -1 on failure                |
//Outputs:        NONE                                                              |
//Description:    Initializes shared memory, semaphore, circular buffer, sets       |
//                up signal handler, and launches DP2 process.                      |
//==================================================================================|
int dp1_init(void) {
    srand(time(NULL));
    sem_id = create_semaphore(SEM_KEY);
    if (sem_id == -1) {
        return -1;
    }

    shm_id = create_shared_memory(SHM_KEY, sizeof(CircularBuffer));
    if (shm_id == -1) {
        return -1;
    }

    cb = (CircularBuffer *)attach_shared_memory(shm_id);
    if (cb == NULL) {
        return -1;
    }

    if (cb_init(cb) == -1) {
        return -1;
    }

    if (setup_signal_handler(SIGINT, dp1_signal_handler) == -1) {
        return -1;
    }

    dp2_pid = dp1_launch_dp2(shm_id);
    if (dp2_pid == -1) {
        return -1;
    }

    return 0;
}

//==================================================FUNCTION========================|
//Name:           dp1_process                                                        |
//Params:         NONE                                                              |
//Returns:        int                     0 when terminated cleanly                 |
//Outputs:        NONE                                                              |
//Description:    Main loop that generates and writes letters to circular buffer.   |
//                Locks/unlocks the semaphore for safe shared memory access.        |
//==================================================================================|
int dp1_process(void) {
    char buffer[20];
    int to_write;

    while (run) {
        dp1_generate_letters(buffer, 20);

        if (lock_semaphore(sem_id) == -1) {
            continue;
        }

        to_write = cb_get_free_space(cb);
        if (to_write > 20) {
            to_write = 20;
        }

        if (to_write > 0) {
            cb_write_multi(cb, buffer, to_write);
        }

        unlock_semaphore(sem_id);
        usleep(DP1_SLEEP_TIME);
    }

    return 0;
}

//==================================================FUNCTION========================|
//Name:           dp1_generate_letters                                               |
//Params:         char *buffer       Buffer to store generated characters            |
//                int count          Number of characters to generate                |
//Returns:        NONE                                                              |
//Outputs:        Fills buffer with characters                                       |
//Description:    Generates 'count' random uppercase characters from CHAR_START to   |
//                CHAR_END and stores them in buffer.                                |
//==================================================================================|
void dp1_generate_letters(char *buffer, int count) {
    int i;
    int range = CHAR_END - CHAR_START + 1;

    for (i = 0; i < count; i++) {
        buffer[i] = CHAR_START + (rand() % range);
    }
}

//==================================================FUNCTION========================|
//Name:           dp1_launch_dp2                                                     |
//Params:         int shm_id         Shared memory ID                                |
//Returns:        pid_t              PID of launched DP2, or -1 on error             |
//Outputs:        NONE                                                              |
//Description:    Forks and executes the DP2 process with shm_id as argument.        |
//==================================================================================|
pid_t dp1_launch_dp2(int shm_id) {
    pid_t pid;
    char shm_id_str[20];

    snprintf(shm_id_str, sizeof(shm_id_str), "%d", shm_id);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        execl(DP2_PROCESS, "dp2", shm_id_str, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    return pid;
}

//==================================================FUNCTION========================|
//Name:           dp1_cleanup                                                        |
//Params:         NONE                                                              |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    Cleans up shared memory and semaphore if this is the last process. |
//==================================================================================|
void dp1_cleanup(void) {
    if (cb != NULL) {
        detach_shared_memory(cb);
        cb = NULL;
    }

    if (dp2_pid == -1) {
        if (shm_id != -1) {
            remove_shared_memory(shm_id);
            shm_id = -1;
        }

        if (sem_id != -1) {
            remove_semaphore(sem_id);
            sem_id = -1;
        }
    }
}

//==================================================FUNCTION========================|
//Name:           dp1_signal_handler                                                 |
//Params:         int sig               Signal value (e.g., SIGINT)                  |
//Returns:        NONE                                                              |
//Outputs:        Sets run to

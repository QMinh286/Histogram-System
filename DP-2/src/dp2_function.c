/*
*	FILE:			dp2.c
*	ASSIGNMENT:		The "Histogram System"
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This file contains the implementation of the DP-2 process,
*					which generates random letters, writes them into the shared
*					buffer, and launches the DC process to visualize the data.
*/

#include "../../common/inc/constants.h"
#include "../../common/inc/circular_buffer.h"
#include "../../common/inc/ipc_utils.h"
#include "../inc/dp2.h"

static CircularBuffer *cb = NULL;  
static int shm_id_g = -1;           
static int sem_id = -1;           
static pid_t dp1_pid = -1;        
static pid_t dc_pid = -1;         
static int run = 1;            

//==================================================FUNCTION========================|
//Name:           dp2_init                                                           |
//Params:         int shm_id              Shared memory ID                          |
//Returns:        int                     0 on success, -1 on failure                |
//Outputs:        NONE                                                              |
//Description:    Initializes DP-2 by setting up shared memory, semaphore, signal    |
//                handling, and launching the DC process.                            |
//==================================================================================|
int dp2_init(int shm_id) {
    shm_id_g = shm_id;
    dp1_pid = getppid();
    srand(time(NULL) ^ getpid());

    sem_id = create_semaphore(SEM_KEY);
    if (sem_id == -1) {
        return -1;
    }

    dc_pid = dp2_launch_dc(shm_id_g, dp1_pid);
    if (dc_pid == -1) {
        return -1;
    }

    cb = (CircularBuffer *)attach_shared_memory(shm_id_g);
    if (cb == NULL) {
        return -1;
    }

    if (setup_signal_handler(SIGINT, dp2_signal_handler) == -1) {
        return -1;
    }

    return 0;
}

//==================================================FUNCTION========================|
//Name:           dp2_process                                                        |
//Params:         NONE                                                              |
//Returns:        int                     0 on normal termination                    |
//Outputs:        Writes letters to circular buffer                                 |
//Description:    Generates letters and writes them to the circular buffer          |
//                until the SIGINT signal is received.                              |
//==================================================================================|
int dp2_process(void) {
    char letter;

    while (run) {
        letter = dp2_generate_letter();

        if (lock_semaphore(sem_id) == -1) {
            continue;
        }

        if (cb_get_free_space(cb) > 0) {
            cb_write_char(cb, letter);
        }

        unlock_semaphore(sem_id);
        usleep(DP2_SLEEP_TIME);
    }

    return 0;
}

//==================================================FUNCTION========================|
//Name:           dp2_generate_letter                                                |
//Params:         NONE                                                              |
//Returns:        char                    Random character in defined range          |
//Outputs:        NONE                                                              |
//Description:    Generates a single random character between CHAR_START and CHAR_END|
//==================================================================================|
char dp2_generate_letter(void) {
    int range = CHAR_END - CHAR_START + 1;
    return CHAR_START + (rand() % range);
}

//==================================================FUNCTION========================|
//Name:           dp2_launch_dc                                                      |
//Params:         int shm_id              Shared memory ID                          |
//                pid_t dp1_pid           Parent process ID                         |
//Returns:        pid_t                   PID of DC process, or -1 on error         |
//Outputs:        Launches DC process                                               |
//Description:    Forks and execs the DC process with required arguments.           |
//==================================================================================|
pid_t dp2_launch_dc(int shm_id, pid_t dp1_pid) {
    pid_t pid;
    char shm_id_str[20];
    char dp1_pid_str[20];
    char dp2_pid_str[20];

    snprintf(shm_id_str, sizeof(shm_id_str), "%d", shm_id);
    snprintf(dp1_pid_str, sizeof(dp1_pid_str), "%d", dp1_pid);
    snprintf(dp2_pid_str, sizeof(dp2_pid_str), "%d", getpid());

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        execl(DC_PROCESS, "dc", shm_id_str, dp1_pid_str, dp2_pid_str, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    return pid;
}

//==================================================FUNCTION========================|
//Name:           dp2_cleanup                                                        |
//Params:         NONE                                                              |
//Returns:        void                                                              |
//Outputs:        NONE                                                              |
//Description:    Detaches the circular buffer from shared memory.                  |
//==================================================================================|
void dp2_cleanup(void) {
    if (cb != NULL) {
        detach_shared_memory(cb);
        cb = NULL;
    }
}

//==================================================FUNCTION========================|
//Name:           dp2_signal_handler                                                 |
//Params:         int sig               Signal received                              |
//Returns:        void                                                              |
//Outputs:        NONE                                                              |
//Description:    Handles SIGINT by setting the run flag to 0 to exit loop.         |
//==================================================================================|
void dp2_signal_handler(int sig) {
    if (sig == SIGINT) {
        run = 0;
    }
}

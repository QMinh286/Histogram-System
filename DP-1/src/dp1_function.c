#include "../../common/include/constants.h"
#include "../../common/include/circular_buffer.h"
#include "../../common/include/ipc_utils.h"
#include "../inc/dp1.h"

static CircularBuffer *cb = NULL;  /* Pointer to shared circular buffer */
static int shm_id = -1;            /* Shared memory ID */
static int sem_id = -1;            /* Semaphore ID */
static pid_t dp2_pid = -1;         /* PID of DP-2 process */
static int run = 1;            /* Flag to control the main loop */

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

/
void dp1_generate_letters(char *buffer, int count) {
    int i;
    int range = CHAR_END - CHAR_START + 1;
    
    for (i = 0; i < count; i++) {
        buffer[i] = CHAR_START + (rand() % range);
    }
}


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


void dp1_cleanup(void) {
    if (cb != NULL) {
        detach_shared_memory(g_cb);
        cb = NULL;
    }
    
    if (dp2_pid == -1) {
        if (shm_id != -1) {
            remove_shared_memory(g_shm_id);
            shm_id = -1;
        }
        
        if (sem_id != -1) {
            remove_semaphore(g_sem_id);
            sem_id = -1;
        }
    }
}

void dp1_signal_handler(int sig) {
    if (sig == SIGINT) {
        run = 0;
    }
}
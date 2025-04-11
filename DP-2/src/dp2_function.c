
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

char dp2_generate_letter(void) {
    int range = CHAR_END - CHAR_START + 1;
    return CHAR_START + (rand() % range);
}

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

void dp2_cleanup(void) {
    if (cb != NULL) {
        detach_shared_memory(cb);
        cb = NULL;
    }
}

void dp2_signal_handler(int sig) {
    if (sig == SIGINT) {
        run = 0;
    }
}
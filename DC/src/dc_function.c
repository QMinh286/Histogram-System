

#include "../../common/inc/constants.h"
#include "../../common/inc/circular_buffer.h"
#include "../../common/inc/ipc_utils.h"
#include "../inc/dc.h"

/* Global variables */
static CircularBuffer *cb = NULL;    
static int shm_id_g = -1;             
static int sem_id = -1;              
static pid_t dp1_pid_g = -1;          
static pid_t dp2_pid_g = -1;          
static int run = 1;              
static int shutdown = 0;             
static int read_count = 0;           
static int letter_counts[26] = {0};  
static int alarm_flag = 0;           


int dc_init(int shm_id, pid_t dp1_pid, pid_t dp2_pid) {
    shm_id_g = shm_id;
    dp1_pid_g = dp1_pid;
    dp2_pid_g = dp2_pid;
    
    sem_id = create_semaphore(SEM_KEY);
    if (sem_id == -1) {
        return -1;
    }
    cb = (CircularBuffer *)attach_shared_memory(shm_id_g);
    if (cb == NULL) {
        return -1;
    }
    
    if (setup_signal_handler(SIGINT, dc_sigint_handler) == -1) {
        return -1;
    }
    
    if (setup_signal_handler(SIGALRM, dc_alarm_handler) == -1) {
        return -1;
    }
    alarm(2); 
    
    return 0;
}

int dc_process(void) {
    while (run) {
        if (alarm_flag) {
            alarm_flag = 0;
            dc_read_data();
            read_count++;
            if (read_count >= DC_DISPLAY_INTERVAL) {
                read_count = 0;
                dc_display_histogram();
            }
            alarm(2);
        }
        
        if (shutdown) {
            if (lock_semaphore(sem_id) != -1) {
                if (cb->read_index == cb->write_index) {
                    unlock_semaphore(sem_id);
                    break;
                }
                unlock_semaphore(sem_id);
            }
        }
        usleep(10000); /* 10ms */
    }
    
    dc_display_histogram();
    dc_exit();
    
    return 0;
}

int dc_read_data(void) {
    char buffer[60]; 
    int read_count = 0;
    int i;
    if (lock_semaphore(sem_id) == -1) {
        return 0;
    }
    
    read_count = cb_read_multi(cb, buffer, 40);
    
    unlock_semaphore(sem_id);
    
    for (i = 0; i < read_count; i++) {
        if (buffer[i] >= CHAR_START && buffer[i] <= CHAR_END) {
            letter_counts[buffer[i] - CHAR_START]++;
        }
    }
    
    return read_count;
}

void dc_display_histogram(void) {
    int i, j;
    int hundreds, tens, ones;
    
    dc_clear_screen();
    
    for (i = 0; i <= (CHAR_END - CHAR_START); i++) {
        hundreds = letter_counts[i] / 100;
        tens = (letter_counts[i] % 100) / 10;
        ones = letter_counts[i] % 10;
        
        printf("%c-%03d ", CHAR_START + i, letter_counts[i]);
        
        for (j = 0; j < hundreds; j++) {
            putchar(HISTOGRAM_HUNDREDS);
        }
        for (j = 0; j < tens; j++) {
            putchar(HISTOGRAM_TENS);
        }
        for (j = 0; j < ones; j++) {
            putchar(HISTOGRAM_ONES);
        }
        
        putchar('\n');
    }
}


void dc_clear_screen(void) {
    printf("\033[2J\033[H");
}

void dc_cleanup(void) {
    if (cb != NULL) {
        detach_shared_memory(cb);
        cb = NULL;
    }
}

void dc_exit(void) {
    dc_cleanup();
    if (shm_id_g != -1) {
        remove_shared_memory(shm_id_g);
        shm_id_g = -1;
    }
    
    if (sem_id != -1) {
        remove_semaphore(sem_id);
        sem_id = -1;
    }
    printf("Shazam !!\n");
}

void dc_sigint_handler(int sig) {
    if (sig == SIGINT) {
        send_signal(dp1_pid_g, SIGINT);
        send_signal(dp2_pid_g, SIGINT);
        shutdown = 1;
    }
}

void dc_alarm_handler(int sig) {
    if (sig == SIGALRM) {
        alarm_flag = 1;
    }
}
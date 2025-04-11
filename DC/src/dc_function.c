/*
*	FILE:			dc.c
*	ASSIGNMENT:	The "Histogram System"
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This file implements the DC (Display Controller) process for the Histogram System. 
*                 It manages reading data from a shared circular buffer, updating a letter frequency histogram, 
*                 and displaying it. It also handles cleanup and shutdown procedures.
*/

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

//==================================================FUNCTION========================|
//Name:           dc_init                                                            |
//Params:         int shm_id            The shared memory ID to be used.             |
//                pid_t dp1_pid        The process ID for DP1.                     |
//                pid_t dp2_pid        The process ID for DP2.                     |
//Returns:        int                    Returns 0 on success, -1 on failure.        |
//Outputs:        NONE                                                              |
//Description:    This function initializes the DC process by creating a semaphore, attaching shared memory, and setting up signal handlers. |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           dc_process                                                         |
//Params:         NONE                                                              |
//Returns:        int                    Returns 0 when completed.                 |
//Outputs:        NONE                                                              |
//Description:    This function runs the DC process in a loop, periodically reading data from the circular buffer and displaying the histogram. |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           dc_read_data                                                       |
//Params:         NONE                                                              |
//Returns:        int                    The number of letters processed.           |
//Outputs:        NONE                                                              |
//Description:    This function reads data from the circular buffer, updates the letter counts based on the read data. |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           dc_display_histogram                                                |
//Params:         NONE                                                              |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    This function displays the histogram of letter frequencies on the screen. |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           dc_clear_screen                                                    |
//Params:         NONE                                                              |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    This function clears the terminal screen.                          |
//==================================================================================|
void dc_clear_screen(void) {
    printf("\033[2J\033[H");
}

//==================================================FUNCTION========================|
//Name:           dc_cleanup                                                         |
//Params:         NONE                                                              |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    This function performs cleanup tasks for the DC process, including detaching shared memory. |
//==================================================================================|
void dc_cleanup(void) {
    if (cb != NULL) {
        detach_shared_memory(cb);
        cb = NULL;
    }
}

//==================================================FUNCTION========================|
//Name:           dc_exit                                                            |
//Params:         NONE                                                              |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    This function performs the final cleanup and shutdown for the DC process. |
//==================================================================================|
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

//==================================================FUNCTION========================|
//Name:           dc_sigint_handler                                                  |
//Params:         int sig               The signal number received.                 |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    This function handles the SIGINT signal by sending a SIGINT to DP1 and DP2, and setting the shutdown flag. |
//==================================================================================|
void dc_sigint_handler(int sig) {
    if (sig == SIGINT) {
        send_signal(dp1_pid_g, SIGINT);
        send_signal(dp2_pid_g, SIGINT);
        shutdown = 1;
    }
}

//==================================================FUNCTION========================|
//Name:           dc_alarm_handler                                                   |
//Params:         int sig               The signal number received.                 |
//Returns:        NONE                                                              |
//Outputs:        NONE                                                              |
//Description:    This function handles the SIGALRM signal by setting the alarm_flag. |
//==================================================================================|
void dc_alarm_handler(int sig) {
    if (sig == SIGALRM) {
        alarm_flag = 1;
    }
}

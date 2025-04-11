/*
*	FILE:			dc_main.c
*	ASSIGNMENT:	The "Histogram System"
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This file serves as the entry point for the DC (Display Controller) process in the Histogram System.
*                 It initializes the necessary resources (shared memory, semaphores, etc.) and runs the DC process.
*/

#include "../inc/dc.h"

int main(int argc, char *argv[]) {
    int result;
    int shm_id;
    pid_t dp1_pid, dp2_pid;
 
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <shm_id> <dp1_pid> <dp2_pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Convert arguments to integers
    shm_id = atoi(argv[1]);
    dp1_pid = atoi(argv[2]);
    dp2_pid = atoi(argv[3]);

    // Initialize DC process
    result = dc_init(shm_id, dp1_pid, dp2_pid);
    if (result != 0) {
        fprintf(stderr, "Failed to initialize DC\n");
        return EXIT_FAILURE;
    }

    // Process the DC functionality
    result = dc_process();
    
    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

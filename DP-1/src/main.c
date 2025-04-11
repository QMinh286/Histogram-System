/*
*	FILE:			main.c
*	ASSIGNMENT:	The "Histogram System"
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This is the entry point for the DP-1 process. It initializes 
*					resources, enters the main processing loop, and performs cleanup 
*					upon termination.
*/

#include "../inc/dp1.h"


int main(int argc, char *argv[]) {
    int result = 0;
    
    result = dp1_init();
    if (result != 0) {
        fprintf(stderr, "Failed to initialize DP-1\n");
        return EXIT_FAILURE;
    }
    result = dp1_process();
    
    dp1_cleanup();
    
    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
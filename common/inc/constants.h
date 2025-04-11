/*
*	FILE:			constants.h
*	ASSIGNMENT:	    Histogram System
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This header file contains global constant definitions used 
*                 throughout the system, including buffer sizes, character ranges, 
*                 semaphore and shared memory keys, process paths, sleep intervals, 
*                 and histogram display characters.
*/
#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Circular buffer size */
#define BUFFER_SIZE 256

#define CHAR_START 'A'
#define CHAR_END 'T'

#define SEM_KEY 0x1234

#define SHM_KEY 0x4321

#define DP1_SLEEP_TIME 2000000 
#define DP2_SLEEP_TIME 50000  
#define DC_READ_SLEEP_TIME 2000000
#define DC_DISPLAY_INTERVAL 5 

#define DP1_PROCESS "/home/qvu5836/assignment5/DP-1/bin/dp1"
#define DP2_PROCESS "/home/qvu5836/assignment5/DP-2/bin/dp2"
#define DC_PROCESS "/home/qvu5836/assignment5/DC/bin/dc"

#define HISTOGRAM_ONES '-'
#define HISTOGRAM_TENS '+'
#define HISTOGRAM_HUNDREDS '*'
#endif /* CONSTANTS_H */

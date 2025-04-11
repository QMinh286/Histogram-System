/*
*	FILE:			circular_buffer.h
*	ASSIGNMENT:	    Histogram System
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This header file defines the structure and dependencies 
*                 required for implementing a circular buffer using shared memory. 
*                 It includes the CircularBuffer struct and necessary system headers.
*/

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

typedef struct {
    int read_index;                
    int write_index;               
    char buffer[256];              
} CircularBuffer;

#endif 
/*
*	FILE:			dp1.h
*	ASSIGNMENT:	The "Histogram System"
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This header file defines the interface for the DP1 (Data Producer 1) process
*                 in the Histogram System. It contains declarations for initialization, processing,
*                 inter-process communication, and signal handling for the DP1 module.
*/
#ifndef DP1_H
#define DP1_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

int dp1_init(void);
int dp1_process(void);
void dp1_generate_letters(char *buffer, int count);
pid_t dp1_launch_dp2(int shm_id);
void dp1_cleanup(void);
void dp1_signal_handler(int sig);

#endif 

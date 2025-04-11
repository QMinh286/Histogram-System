
#ifndef DP2_H
#define DP2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int dp2_init(int shm_id);
int dp2_process(void);
char dp2_generate_letter(void);
pid_t dp2_launch_dc(int shm_id, pid_t dp1_pid);
void dp2_signal_handler(int sig);

#endif 

#ifndef DC_H
#define DC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int dc_init(int shm_id, pid_t dp1_pid, pid_t dp2_pid);
int dc_process(void);
int dc_read_data(void);
void dc_display_histogram(void);
void dc_clear_screen(void);
void dc_cleanup(void);
void dc_exit(void);
void dc_sigint_handler(int sig);
void dc_alarm_handler(int sig);

#endif /* DC_H */
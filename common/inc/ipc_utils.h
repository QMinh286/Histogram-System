/**
 * @file ipc_utils.h
 * @brief IPC utility functions for the HISTO-SYSTEM
 */

#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <sys/types.h>
#include "circular_buffer.h"


int create_semaphore(key_t sem_key);

int lock_semaphore(int sem_id);

int unlock_semaphore(int sem_id);

int remove_semaphore(int sem_id);

int create_shared_memory(key_t shm_key, size_t size);

void *attach_shared_memory(int shm_id);

int detach_shared_memory(const void *ptr);

int remove_shared_memory(int shm_id);

int send_signal(pid_t pid, int sig);

int setup_signal_handler(int sig, void (*handler)(int));

int setup_alarm_handler(void (*handler)(int), unsigned int interval);

#endif 

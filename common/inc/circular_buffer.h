#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h> 
#include <string.h>

/**
 * Circular buffer data structure
 * This will be stored in shared memory
 */
typedef struct {
    int read_index;                /* Index for reading from the buffer */
    int write_index;               /* Index for writing to the buffer */
    char buffer[256];              /* Actual buffer data - fixed size 256 */
} CircularBuffer;

#endif 
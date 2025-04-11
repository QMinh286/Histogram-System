/*
*	FILE:			circular_buffer.c
*	ASSIGNMENT:     Histogram System
*	PROGRAMMERS:	Quang Minh Vu
*	DESCRIPTION:	This file implements the functions for managing a circular buffer, 
*                 providing functionality for reading, writing, and managing buffer space.
*/
#include "../inc/circular_buffer.h"
#include "../inc/constants.h"

//==================================================FUNCTION========================|
//Name:           cb_init                                                            |
//Params:         CircularBuffer* cb      A pointer to the circular buffer to be initialized. |
//Returns:        int                     Returns 0 on success, -1 on failure.     |
//Outputs:        NONE                                                              |
//Description:    This function initializes the circular buffer by setting the read and write |
//                indices to 0 and clearing the buffer.                               |
//==================================================================================|
int cb_init(CircularBuffer *cb) {
    if (!cb) {
        return -1;
    }
    
    cb->read_index = 0;
    cb->write_index = 0;
    memset(cb->buffer, 0, BUFFER_SIZE);
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           cb_write_char                                                      |
//Params:         CircularBuffer* cb      A pointer to the circular buffer to write to. |
//                char c                  The character to write to the buffer.       |
//Returns:        int                     Returns 0 on success, -1 if buffer is full. |
//Outputs:        NONE                                                              |
//Description:    This function writes a single character to the circular buffer. If the buffer |
//                is full, it returns -1. Otherwise, it writes the character and updates the write index. |
//==================================================================================|
int cb_write_char(CircularBuffer *cb, char c) {
    if (!cb) {
        return -1;
    }
    
    if ((cb->write_index + 1) % BUFFER_SIZE == cb->read_index) {
        return -1; 
    }
    
    cb->buffer[cb->write_index] = c;
    cb->write_index = (cb->write_index + 1) % BUFFER_SIZE;
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           cb_read_char                                                       |
//Params:         CircularBuffer* cb      A pointer to the circular buffer to read from. |
//                char* c                 A pointer to store the read character.      |
//Returns:        int                     Returns 0 on success, -1 if buffer is empty. |
//Outputs:        NONE                                                              |
//Description:    This function reads a single character from the circular buffer. If the buffer is empty, |
//                it returns -1. Otherwise, it reads the character and updates the read index. |
//==================================================================================|
int cb_write_multi(CircularBuffer *cb, const char *data, size_t len) {
    if (!cb || !data) {
        return -1;
    }
    
    int written = 0;
    for (size_t i = 0; i < len; i++) {
        if (cb_write_char(cb, data[i]) == 0) {
            written++;
        } else {
            break;  /* Buffer full */
        }
    }
    
    return written;
}


//==================================================FUNCTION========================|
//Name:           cb_read_char                                                       |
//Params:         CircularBuffer* cb      A pointer to the circular buffer to read from. |
//                char* c                 A pointer to store the read character.      |
//Returns:        int                     Returns 0 on success, -1 if buffer is empty. |
//Outputs:        NONE                                                              |
//Description:    This function reads a single character from the circular buffer. If the buffer is empty, |
//                it returns -1. Otherwise, it reads the character and updates the read index. |
//==================================================================================|
int cb_read_char(CircularBuffer *cb, char *c) {
    if (!cb || !c) {
        return -1;
    }
    if (cb->read_index == cb->write_index) {
        return -1;  
    }
    
    *c = cb->buffer[cb->read_index];
    cb->read_index = (cb->read_index + 1) % BUFFER_SIZE;
    
    return 0;
}

//==================================================FUNCTION========================|
//Name:           cb_read_multi                                                      |
//Params:         CircularBuffer* cb      A pointer to the circular buffer to read from. |
//                char* buf               A buffer to store the read characters.     |
//                size_t max_len          The maximum number of characters to read. |
//Returns:        int                     The number of successfully read characters. |
//Outputs:        NONE                                                              |
//Description:    This function reads multiple characters from the circular buffer. It returns the 
//                number of successfully read characters, stopping if the buffer is empty. |
//==================================================================================|
int cb_read_multi(CircularBuffer *cb, char *buf, size_t max_len) {
    if (!cb || !buf) {
        return -1;
    }
    
    int read = 0;
    for (size_t i = 0; i < max_len; i++) {
        if (cb_read_char(cb, &buf[i]) == 0) {
            read++;
        } else {
            break;  
        }
    }
    
    return read;
}

//==================================================FUNCTION========================|
//Name:           cb_get_available                                                   |
//Params:         const CircularBuffer* cb   A pointer to the circular buffer to check. |
//Returns:        int                     The number of available characters in the buffer. |
//Outputs:        NONE                                                              |
//Description:    This function returns the number of available characters in the buffer,
//                i.e., the number of characters that have been written but not yet read. |
//==================================================================================|
int cb_get_available(const CircularBuffer *cb) {
    if (!cb) {
        return -1;
    }
    
    if (cb->write_index >= cb->read_index) {
        return cb->write_index - cb->read_index;
    } else {
        return BUFFER_SIZE - (cb->read_index - cb->write_index);
    }
}

//==================================================FUNCTION========================|
//Name:           cb_get_free_space                                                  |
//Params:         const CircularBuffer* cb   A pointer to the circular buffer to check. |
//Returns:        int                     The number of free spaces available in the buffer. |
//Outputs:        NONE                                                              |
//Description:    This function returns the number of free spaces available in the buffer,
//                i.e., the number of characters that can still be written. |
//==================================================================================|
int cb_get_free_space(const CircularBuffer *cb) {
    if (!cb) {
        return -1;
    }
    
    return BUFFER_SIZE - cb_get_available(cb) - 1;
}

#include "../inc/circular_buffer.h"
#include "../inc/constants.h"


int cb_init(CircularBuffer *cb) {
    if (!cb) {
        return -1;
    }
    
    cb->read_index = 0;
    cb->write_index = 0;
    memset(cb->buffer, 0, BUFFER_SIZE);
    
    return 0;
}

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


int cb_get_free_space(const CircularBuffer *cb) {
    if (!cb) {
        return -1;
    }
    
    return BUFFER_SIZE - cb_get_available(cb) - 1;
}

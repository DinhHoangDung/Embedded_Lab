/*
 * ring_buffer.c
 *
 *  Created on: Nov 17, 2024
 *      Author: HOANG DUNG
 */
#include "ring_buffer.h"

void buffer_init(RingBuffer *buf) {
    buf->head = 0;
    buf->tail = 0;
    buf->full = false;
}

bool buffer_is_empty(RingBuffer *buf) {
    return (buf->head == buf->tail) && !buf->full;
}

bool buffer_is_full(RingBuffer *buf) {
    return buf->full;
}

bool buffer_put(RingBuffer *buf, uint8_t data) {
    if (buffer_is_full(buf)) {
        return false; // Buffer overflow
    }

    buf->buffer[buf->head] = data;
    buf->head = (buf->head + 1) % BUFFER_SIZE;
    buf->full = (buf->head == buf->tail);

    return true; // Success
}

bool buffer_get(RingBuffer *buf, uint8_t *data) {
    if (buffer_is_empty(buf)) {
        return false;  // No data to read
    }

    *data = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1) % BUFFER_SIZE;
    buf->full = false;

    return true;  // Read successfully
}


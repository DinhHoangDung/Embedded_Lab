/*
 * ring_buffer.h
 *
 *  Created on: Nov 17, 2024
 *      Author: HOANG DUNG
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 128  // Buffer size

typedef struct {
    uint8_t buffer[BUFFER_SIZE];  // Data array
    uint16_t head;               // Write position
    uint16_t tail;               // Read position
    bool full;                   // isFull flag
} RingBuffer;

void buffer_init(RingBuffer *buf);
bool buffer_is_empty(RingBuffer *buf);
bool buffer_is_full(RingBuffer *buf);
bool buffer_put(RingBuffer *buf, uint8_t data);
bool buffer_write(RingBuffer *buf, uint8_t *data);

#endif /* INC_RING_BUFFER_H_ */

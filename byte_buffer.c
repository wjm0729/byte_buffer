/*
 * Created by Alan Lei
 *
 * C implementation of Java ByteBuffer
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "byte_buffer.h"

byte_buffer_t *bb_create(size_t max) {
    byte_buffer_t *bb = (byte_buffer_t *)malloc(sizeof(byte_buffer_t));
    memset(bb, 0, sizeof(byte_buffer_t));
    bb_clear(bb);
    bb->capacity = max;
    bb->mark = -1;
    bb->position = 0;
    bb->limit = max;
    bb->buffer = (char*)malloc(bb->capacity);
    if (bb->buffer == NULL) {
        printf("allocate problem\n");
    }
    bb->isWrapped = 0;
    bb->order = BYTE_ORDER_BIG_ENDIAN;
    return bb;
}

byte_buffer_t *bb_wrap(char *src, size_t offset, size_t length) {
    byte_buffer_t *bb = (byte_buffer_t *)malloc(sizeof(byte_buffer_t));
    memset(bb, 0, sizeof(byte_buffer_t));
    bb_clear(bb);
    bb->buffer = src + offset;
    bb->limit = bb->capacity = length;
    bb->isWrapped = 1;
    bb->order = BYTE_ORDER_BIG_ENDIAN;
    return bb;
}

void bb_destroy(byte_buffer_t *bb) {
    if (!bb->isWrapped) {
        free(bb->buffer);
    }
    free(bb);
}

byte_order_t bb_order(byte_buffer_t *bb) {
    return bb->order;
}

void bb_order_set(byte_buffer_t *bb, byte_order_t order) {
    bb->order = order;
}

byte_buffer_t *bb_compact(byte_buffer_t *bb) {
    size_t remain = bb_remaining(bb);
    if (bb->position > 0) {
        memcpy(bb->buffer, bb->buffer + bb->position, remain);
    }
    bb->position = remain;
    bb->limit = bb->capacity;
    bb->mark = -1;
    return bb;
}


void bb_clear(byte_buffer_t *bb) {
    bb->position = 0;
    bb->limit = bb->capacity;
    bb->mark = -1;
}

void bb_rewind(byte_buffer_t *bb) {
    bb->position = 0;
}

void bb_flip(byte_buffer_t *bb) {
    bb->limit = bb->position;
    bb->position = 0;
}

size_t bb_limit(byte_buffer_t *bb) {
    return bb->limit;
}

void bb_limit_set(byte_buffer_t *bb, size_t pos) {
    assert(pos >= 0);
    assert(pos <= bb->capacity);
    bb->limit = pos;
    if (bb->position > pos) bb->position = pos;
    if (bb->mark > pos) bb->mark = -1;
}

size_t bb_position(byte_buffer_t *bb) {
    return bb->position;
}

void bb_position_set(byte_buffer_t *bb, size_t pos) {
    assert(pos >= 0);
    assert(pos <= bb->limit);
    bb->position = pos;
    if (bb->mark > pos) bb->mark = -1;
}

char *bb_array(byte_buffer_t *bb) {
    return bb->buffer;
}

size_t bb_remaining(byte_buffer_t *bb) {
    return bb->limit - bb->position;
}

void bb_mark(byte_buffer_t *bb) {
    bb->mark = bb->position;
}

void bb_reset(byte_buffer_t *bb) {
    assert(bb->mark != -1);
    bb->position = bb->mark;
}

size_t bb_capacity(byte_buffer_t *bb) {
    return bb->capacity;
}


void bb_put(byte_buffer_t *bb, char c) {
    assert(bb->position < bb->limit);
    bb->buffer[bb->position] = c;
    bb->position++;
    if (bb->position > bb->limit) bb->limit = bb->position;
}

void bb_put_index(byte_buffer_t *bb, size_t index, char c) {
    assert(index < bb->capacity);
    bb->buffer[index] = c;
}

void bb_put_bb(byte_buffer_t *bb, byte_buffer_t *src) {
    assert(bb_remaining(bb) >= bb_remaining(src));
    while (bb_remaining(src) > 0) {
        bb_put(bb, bb_get(src));
    }
}

void bb_put_buffer(byte_buffer_t *bb, char *src, size_t offset, size_t length) {
    assert((bb->position + length) < bb->limit);
    memcpy(bb->buffer + bb->position, src + offset, length);
    bb->position += length;
    if (bb->position > bb->limit) bb->limit = bb->position;
}

void bb_putShort(byte_buffer_t *bb, int16_t value) {
    assert(bb_remaining(bb) >= 2);
    if (bb->order == BYTE_ORDER_BIG_ENDIAN) {
        bb->buffer[bb->position] = (value >> 8) & 0xFF;
        bb->buffer[bb->position + 1] = value & 0xFF;
    }
    else {
        bb->buffer[bb->position + 1] = (value >> 8) & 0xFF;
        bb->buffer[bb->position] = value & 0xFF;
    }
    bb->position += 2;
    if (bb->position > bb->limit) bb->limit = bb->position;
}

void bb_putInt(byte_buffer_t *bb, int32_t value) {
    assert(bb_remaining(bb) >= 4);
    
    if (bb->order == BYTE_ORDER_BIG_ENDIAN) {
        bb->buffer[bb->position] = (value >> 24) & 0xFF;
        bb->buffer[bb->position + 1] = (value >> 16) & 0xFF;
        bb->buffer[bb->position + 2] = (value >> 8) & 0xFF;
        bb->buffer[bb->position + 3] = value & 0xFF;
    }
    else {
        bb->buffer[bb->position + 3] = (value >> 24) & 0xFF;
        bb->buffer[bb->position + 2] = (value >> 16) & 0xFF;
        bb->buffer[bb->position + 1] = (value >> 8) & 0xFF;
        bb->buffer[bb->position] = value & 0xFF;
    }
    bb->position += 4;
    if (bb->position > bb->limit) bb->limit = bb->position;
}

void bb_putLong(byte_buffer_t *bb, int64_t value) {
    assert(bb_remaining(bb) >= 8);
    
    if (bb->order == BYTE_ORDER_BIG_ENDIAN) {
        bb->buffer[bb->position] = (value >> 56) & 0xFF;
        bb->buffer[bb->position + 1] = (value >> 48) & 0xFF;
        bb->buffer[bb->position + 2] = (value >> 40) & 0xFF;
        bb->buffer[bb->position + 3] = (value >> 32) & 0xFF;
        bb->buffer[bb->position + 4] = (value >> 24) & 0xFF;
        bb->buffer[bb->position + 5] = (value >> 16) & 0xFF;
        bb->buffer[bb->position + 6] = (value >> 8) & 0xFF;
        bb->buffer[bb->position + 7] = value & 0xFF;
    }
    else {
        bb->buffer[bb->position + 7] = (value >> 56) & 0xFF;
        bb->buffer[bb->position + 6] = (value >> 48) & 0xFF;
        bb->buffer[bb->position + 5] = (value >> 40) & 0xFF;
        bb->buffer[bb->position + 4] = (value >> 32) & 0xFF;
        bb->buffer[bb->position + 3] = (value >> 24) & 0xFF;
        bb->buffer[bb->position + 2] = (value >> 16) & 0xFF;
        bb->buffer[bb->position + 1] = (value >> 8) & 0xFF;
        bb->buffer[bb->position] = value & 0xFF;
    }
    bb->position += 8;
    if (bb->position > bb->limit) bb->limit = bb->position;
}


char bb_get(byte_buffer_t *bb) {
    size_t pos = bb->position;
    bb->position++;
    return bb->buffer[pos];
}

char bb_get_index(byte_buffer_t *bb, size_t index) {
    assert(index >= 0);
    assert(index < bb->capacity);
    return bb->buffer[index];
}

void bb_get_buffer(byte_buffer_t *bb, char *dst, size_t offset, size_t length) {
    assert(offset >= 0);
    assert(length >= 0);
    assert(bb_remaining(bb) >= length);
    memcpy(dst + offset, bb->buffer + bb->position, length);
    bb->position += length;
}

int16_t bb_getShort(byte_buffer_t *bb) {
    assert(bb_remaining(bb) >= 2);

    int16_t value;
    if (bb->order == BYTE_ORDER_BIG_ENDIAN) {
        value = (bb->buffer[bb->position] & 0xFF) << 8;
        value |= (bb->buffer[bb->position + 1] & 0xFF);
    }
    else {
        value = (bb->buffer[bb->position + 1] & 0xFF) << 8;
        value |= (bb->buffer[bb->position] & 0xFF);
    }
    bb->position += 2;
    return value;
}

int32_t bb_getInt(byte_buffer_t *bb) {
    assert(bb_remaining(bb) >= 4);
    
    int32_t value;
    if (bb->order == BYTE_ORDER_BIG_ENDIAN) {
        value = (bb->buffer[bb->position] & 0xFF) << 24;
        value |= (bb->buffer[bb->position + 1] & 0xFF) << 16;
        value |= (bb->buffer[bb->position + 2] & 0xFF) << 8;
        value |= bb->buffer[bb->position + 3] & 0xFF;
    }
    else {
        value = (bb->buffer[bb->position + 3] & 0xFF) << 24;
        value |= (bb->buffer[bb->position + 2] & 0xFF) << 16;
        value |= (bb->buffer[bb->position + 1] & 0xFF) << 8;
        value |= bb->buffer[bb->position] & 0xFF;
    }
    bb->position += 4;
    return value;
}

int64_t bb_getLong(byte_buffer_t *bb) {
    assert(bb_remaining(bb) >= 8);

    int64_t value;
    if (bb->order == BYTE_ORDER_BIG_ENDIAN) {
        value = ((int64_t)bb->buffer[bb->position] & 0xFF) << 56;
        value |= ((int64_t)bb->buffer[bb->position + 1] & 0xFF) << 48;
        value |= ((int64_t)bb->buffer[bb->position + 2] & 0xFF) << 40;
        value |= ((int64_t)bb->buffer[bb->position + 3] & 0xFF) << 32;
        value |= (bb->buffer[bb->position + 4] & 0xFF) << 24;
        value |= (bb->buffer[bb->position + 5] & 0xFF) << 16;
        value |= (bb->buffer[bb->position + 6] & 0xFF) << 8;
        value |= (bb->buffer[bb->position + 7] & 0xFF);
    }
    else {
        value = ((int64_t)bb->buffer[bb->position + 7] & 0xFF) << 56;
        value |= ((int64_t)bb->buffer[bb->position + 6] & 0xFF) << 48;
        value |= ((int64_t)bb->buffer[bb->position + 5] & 0xFF) << 40;
        value |= ((int64_t)bb->buffer[bb->position + 4] & 0xFF) << 32;
        value |= (bb->buffer[bb->position + 3] & 0xFF) << 24;
        value |= (bb->buffer[bb->position + 2] & 0xFF) << 16;
        value |= (bb->buffer[bb->position + 1] & 0xFF) << 8;
        value |= (bb->buffer[bb->position] & 0xFF);
    }
    bb->position += 8;
    return value;
}

/*
 * Created by Alan Lei
 *
 * To test byte_buffer
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "byte_buffer.h"

#define ASSERT_BB(buffer, pos, remain, limit) \
  assert(bb_position(buffer) == pos); \
  assert(bb_remaining(buffer) == remain); \
  assert(bb_limit(buffer) == limit);

int main(int argc, const char * argv[]) {
    byte_buffer_t *bb = bb_create(15);
    assert(bb_capacity(bb) == 15);
    
    ASSERT_BB(bb, 0, 15, 15);

    bb_put_buffer(bb, "Hello", 0, 5);
    ASSERT_BB(bb, 5, 10, 15);

    bb_flip(bb);
    ASSERT_BB(bb, 0, 5, 5);
    
    bb_get(bb);
    ASSERT_BB(bb, 1, 4, 5);

    bb_mark(bb);
    bb_get(bb);
    bb_get(bb);
    bb_get(bb);
    bb_get(bb);
    ASSERT_BB(bb, 5, 0, 5);

    bb_reset(bb);
    ASSERT_BB(bb, 1, 4, 5);
    
    assert(memcmp("Hello", bb_array(bb), 5) == 0);
    
    byte_buffer_t *bb2 = bb_create(16);
    bb_put_buffer(bb2, "There", 0, 5);
    bb_flip(bb2);
    
    bb_position_set(bb, 0);
    bb_compact(bb);
    ASSERT_BB(bb, 5, 10, 15);

    bb_put_bb(bb, bb2);
    bb_flip(bb);
    ASSERT_BB(bb, 0, 10, 10);
    
    assert(memcmp("HelloThere", bb_array(bb), 10) == 0);

    bb_clear(bb);
    ASSERT_BB(bb, 0, 15, 15);
    
    // Order tests. Using ASCII 1-8
    long ascii1to8 = 0x3132333435363738L;
    bb_putLong(bb, ascii1to8);
    bb_flip(bb);
    assert(bb_order(bb) == BYTE_ORDER_BIG_ENDIAN);
    assert(bb_getLong(bb) == ascii1to8);
    assert(memcmp("12345678", bb_array(bb), 8) == 0);

    bb_clear(bb);
    bb_order_set(bb, BYTE_ORDER_LITTLE_ENDIAN);
    bb_putLong(bb, ascii1to8);
    bb_flip(bb);
    assert(bb_order(bb) == BYTE_ORDER_LITTLE_ENDIAN);
    assert(bb_getLong(bb) == ascii1to8);
    assert(memcmp("87654321", bb_array(bb), 8) == 0);

    return 0;
}

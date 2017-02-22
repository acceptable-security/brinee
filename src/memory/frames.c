#include <stdlib.h>
#include <memory/frames.h>
#include <memory/kmem.h>

uint32_t* frame_map;
uint32_t frame_cnt;

uintptr_t frame_test(int n) {
    return TEST_FRAME_MAP(n);
}

uintptr_t frame_first() {
    for ( uint32_t i = 0; i < frame_cnt; i++ ) {
        if ( frame_map[i] != 0xFFFFFFFF ) {
            for ( int j = 0; j < 32; j++ ) {
                if ( !(frame_map[INDEX_FROM_BIT(i)] & (1 << j)) ) {
                    return i * 0x20 + j;
                }
            }
        }
    }

    bsod("Ran out of frames for allocation");
    return 0;
}

uintptr_t frame_firstn(int n) {
    for ( uint32_t i = 0; i < frame_cnt; i++ ) {
        bool found = true;

        for ( uint32_t j = 0; j < n; j++ ) {
            if ( TEST_FRAME_MAP((i + j) * 0x1000) ) {
                found = false;
                break;
            }
        }

        if ( found ) {
            return i;
        }
    }

    bsod("Ran out of frames for allocation");
    return 0;
}

void frame_set(uintptr_t frame_addr, bool set) {
    uint32_t addr = frame_addr / 0x1000;

    if ( set ) {
        frame_map[INDEX_FROM_BIT(addr)] |= (1 << OFFSET_FROM_BIT(addr));
    }
    else {
        frame_map[INDEX_FROM_BIT(addr)] &= ~(1 << OFFSET_FROM_BIT(addr));
    }
}

uint32_t frame_used() {
    uint32_t out = 0;

    for ( uint32_t i = 0; i < frame_cnt; i++ ) {
        if ( TEST_FRAME_MAP(i) ) {
            out++;
        }
    }

    return out;
}

uint32_t frame_total() {
    return frame_cnt * 4;
}

void frame_init(uint32_t memsize) {
    frame_cnt = memsize / 4;
    frame_map = (uint32_t*) kmalloc(INDEX_FROM_BIT(frame_cnt * 8));
}

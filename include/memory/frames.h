#include <stdint.h>
#include <stdbool.h>

#define INDEX_FROM_BIT(b) (b / 0x20)
#define OFFSET_FROM_BIT(b) (b % 0x20)

#define TEST_FRAME_MAP(I) (frame_map[INDEX_FROM_BIT(I)] & (1 << OFFSET_FROM_BIT(I)))

uintptr_t frame_test(int n);
uintptr_t frame_first();
uintptr_t frame_firstn(int n);
void frame_set(uintptr_t frame_addr, bool set);
uint32_t frame_used();
uint32_t frame_total();
void frame_init(uint32_t memsize);

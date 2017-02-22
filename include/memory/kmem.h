#include <stdint.h>
#include <stdbool.h>

#define NULL (uintptr_t) 0
#define KHEAP_AMT 0x100000

uintptr_t kmalloc(uint32_t size);
uintptr_t kvmalloc(uint32_t size);
uintptr_t kmalloc_p(uint32_t size, uintptr_t* phys);
uintptr_t kvmalloc_p(uint32_t size, uintptr_t *phys);
uintptr_t kernel_heap_bottom();
void kernel_flush_heap();
void kernel_heap_install(uintptr_t kern_end);
uintptr_t kernel_heap_switch();

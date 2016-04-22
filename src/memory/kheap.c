#include <memory/kmem.h>

void* kheap_start;
void* kheap_current;
void* kheap_end;

void* kmalloc(unsigned int amnt) {
    void* mem = kheap_current;
    kheap_current += amnt;

    if ( kheap_current > kheap_end ) {
        bsod("Ran out of physical memory for the kernel.", 0);
    }

    return mem;
}

void kernel_flush_heap() {
    memset(kheap_start, 0, KHEAP_AMT);
    kheap_current = kheap_start;
}

void* kernel_heap_bottom() {
    return kheap_end;
}

void kernel_heap_install(void* kern_end) {
    kheap_start = kern_end + 1; // + 1 not necessary. just in case.
    kheap_current = kheap_start;
    kheap_end = kheap_start + KHEAP_AMT;
    kernel_flush_heap();
}

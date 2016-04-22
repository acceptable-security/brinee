#include <memory/kmem.h>

void liballoc_lock() {
    __asm__ ("cli");
}

void liballoc_unlock() {
    __asm__ ("sti");
}

void* liballoc_alloc(int amt) {
    return kmalloc(amt * 4096);
}

extern int liballoc_free(void* page, int num) {
    return 0; // TODO
}

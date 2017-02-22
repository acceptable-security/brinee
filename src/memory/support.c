#include <memory/kmem.h>
#include <memory/paging.h>

void liballoc_lock() {
    __asm__ ("cli");
}

void liballoc_unlock() {
    __asm__ ("sti");
}

void* liballoc_alloc(int amt) {
    return page_alloc(amt);
}

extern int liballoc_free(void* page, int num) {
    page_free(page, num);
    return 0;
}

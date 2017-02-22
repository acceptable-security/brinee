#include <memory/kmem.h>
#include <memory/frames.h>
#include <memory/paging.h>

page_directory_t* current_directory;

void paging_set_directory(page_directory_t* dir) {
    __asm__ volatile("mov %0, %%eax\n"
                     "mov %%eax, %%cr3\n"
                     : : "r"(dir));
}

void paging_alloc(page_t* page, bool user, bool rw) {
    if ( page->frame != 0 ) {
        page->present = 1;
        page->rw = rw;
        page->user = user;
    }
    else {
        uint32_t index = 0x00100000 + frame_first();
        frame_set(index, 1);
        page->frame = index;
        paging_alloc(page, user, rw);
    }
}

void paging_alloc_dma(page_t* page, bool user, bool rw, uintptr_t addr) {
    page->present = 1;
    page->rw = rw;
    page->user = user;
    page->frame = addr / 0x1000;
}

uintptr_t page_alloc(uint32_t amount) {
    uint32_t addr = 0x00100000 + frame_firstn(amount) * 0x1000;

    for ( uint32_t i = addr; i < addr + amount * 0x1000; i += 0x1000 ) {
        if ( frame_test(i) ) {
            bsod("Received an invalid frame.");
        }

        paging_alloc_dma(paging_acquire(current_directory, i, 1), false, false, i);
    }

    return addr;
}

void page_free(uint32_t page, uint32_t amount) {
    while ( amount-- ) {
        paging_release(page -= 0x1000);
    }
}

page_t* paging_acquire(page_directory_t* dir, uintptr_t addr, bool init) {
    uint32_t index = addr / 0x1000;
    uint32_t table_index = addr / 1024;

    if ( dir->tables[table_index] ) {
        return &dir->tables[table_index]->pages[index % 1024];
    }
    else if ( init ) {
        uint32_t tmp;
        dir->tables[table_index] = (page_table_t*) kvmalloc_p(sizeof(page_table_t), (uintptr_t*) &tmp);
        memset(dir->tables[table_index], 0, sizeof(page_table_t));
        dir->physical_tables[table_index] = tmp | 0x7;
        return &dir->tables[table_index]->pages[index % 1024];
    }
    else {
        bsod("OH FUCK!", NULL);
        return NULL;
    }
}

void paging_release(page_t* page) {
    if ( !page->frame ) {
        bsod("Page failed to delocate");
        return;
    }

    frame_set(page->frame * 0x1000, false);
    page->frame = 0;
}

page_directory_t* paging_new_directory() {
    page_directory_t* dir = (page_directory_t*) kmalloc_p(sizeof(page_directory_t), NULL);
    memset(dir, 0, sizeof(page_directory_t));
    return dir;
}

void paging_fault(struct regs* r) {
    bsod("Paging fault", r);
}

uintptr_t virtual_to_physical(uintptr_t addr) {
    uintptr_t rem = addr % 0x1000;
    uintptr_t f = addr / 0x1000;
    uintptr_t t = f / 1024;
    uintptr_t i = f % 1024;

    if ( current_directory->tables[t] ) {
        return (current_directory->tables[t]->pages[i]).frame * 0x1000 + rem;
    }

    return 0;
}

void paging_install(uintptr_t kernel_end, uint32_t mem_size) {
    current_directory = paging_new_directory();

    paging_alloc_dma(paging_acquire(current_directory, 0, true), false, false, 0);

    printf("Allocating page directory at %08x\n", current_directory);

    for ( uintptr_t i = 0; i < kernel_end; i += 0x1000) {
        paging_alloc_dma(paging_acquire(current_directory, i, true), false, false, i);
    }

    printf("A\n");

    for ( uintptr_t i = 0xB8000; i < 0xC0000; i += 0x1000 ) {
        paging_alloc_dma(paging_acquire(current_directory, i, true), false, true, i);
    }

    printf("B\n");

    printf("D\n");


    printf("MM: Paging directory complete.\n");
    paging_set_directory(current_directory);

    __asm__ volatile("mov %%cr0, %%eax\n"
        			 "or $0x80000000, %%eax\n"
                     "mov %%eax, %%cr0\n" : :);

    for(;;){}
    irq_install_handler(14, paging_fault);
}

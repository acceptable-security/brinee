#include <stdint.h>
#include <memory/kmem.h>

typedef struct page_t {
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct {
    uint32_t pages[1024];
} page_table_t;

typedef struct {
    page_table_t* tables[1024];
} page_directory_t;

uint32_t paging_acquire_frame() {
    return (uint32_t) kmalloc(4096); // How to handle during paging?
}

void paging_set_directory(page_directory_t* dir) {
    __asm__ volatile("mov %0, %%eax\n"
                     "mov %%eax, %%cr3\n"
                     : : "r"(&dir->tables));
}

void paging_release_page(page_directory_t* dir, void* addr) {
    unsigned int table = ((uint32_t) addr) >> 22;
    unsigned int page = ((uint32_t) addr) >> 12 & 0x03FF;

    if ( dir->tables[table] != 0 ) {
        return;
    }

    dir->tables[table]->pages[page] = 0;
}

void* paging_acquire_page(page_directory_t* dir, unsigned int access) {
    if ( dir == 0 ) {
        return 0;
    }

    for ( int i = 0; i < 1023; i++ ) { // Ignore last, it has BOOGGERS
        if (  dir->tables[i] != 0 ) {
            for ( int j = 0; j < 1024; j++ ) {
                if ( dir->tables[i]->pages[j] == 0 ) {
                    dir->tables[i]->pages[j] = paging_acquire_frame() | access;
                    return i << 22 | (j & 0x03FF) << 12;
                }
            }
        }
        else {
            dir->tables[i] = (page_table_t*) kmalloc(sizeof(page_table_t));
            memset(dir->tables[i], 0, sizeof(page_table_t));
            dir->tables[i]->pages[0] = paging_acquire_frame() | access;
            return i << 22 | (0 & 0x03FF) << 12;
        }
    }

    return 0;
}

page_directory_t* paging_new_directory() {
    page_directory_t* dir = (page_directory_t*) kmalloc(sizeof(page_directory_t));
    memset(dir, 0, sizeof(page_directory_t));
    dir->tables[1023] = (page_table_t*) kmalloc(sizeof(page_table_t));

    for ( int i = 0; i < 1024; i++ ) {
        dir->tables[1023]->pages[i] = (uint32_t)((i * 0x1000) | 0x3);
    }

    return dir;
}

void paging_fault(struct regs* r) {
    bsod("Paging fault", r);
}

void paging_install(unsigned int* mem_end, unsigned int mem_size) {
    page_directory_t* p = paging_new_directory();

    irq_install_handler(14, paging_fault);

    __asm__ volatile("mov %0, %%eax\n"
        			 "mov %%eax, %%cr3\n"
        			 "mov $0x20, %%dx\n"
        			 "mov %%cr0, %%eax\n"
        			 "or $0x80000000, %%eax\n"
                     "mov %%eax, %%cr0\n"
			         : : "r"(&p->tables));
}

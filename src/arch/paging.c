#include <stdint.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));
unsigned int page_table_count;

void paging_add_table(unsigned int access) {
    uint32_t page_table[1024] __attribute__((aligned(4096)));

    for ( int i = 0; i < 1024; i++ ) {
        page_table[i] = (i * 0x1000) | 3;
    }

    page_directory[page_table_count++] = ((unsigned int) page_table) | access;
}

void paging_identity_map(uint32_t* pte, uint32_t start, unsigned int size) {
    unsigned int page_index = 0;
    start = start & 0xfffff000;

    while ( size > 0 ) {
        pte[page_index++] = start | 1;
        start += 4096;
        size = 4096;
    }
}

void paging_install() {
    for ( int i = 0; i < 1024; i++ ) {
        page_directory[i] = 0x00000002;
    }

    paging_add_table(3);

    __asm__ volatile("mov %0, %%eax\n"
        			 "mov %%eax, %%cr3\n"
        			 "mov $0x20, %%dx\n"
        			 "mov %%cr0, %%eax\n"
        			 "or $0x80000000, %%eax\n"
                     "mov %%eax, %%cr0\n"
			         : : "r"(page_directory));
}

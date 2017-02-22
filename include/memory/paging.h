#include <stdint.h>
#include <stdbool.h>

typedef struct page_t {
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed)) page_t;

typedef struct {
    page_t pages[1024];
} __attribute__((packed, aligned(4096))) page_table_t;

typedef struct {
    page_table_t* tables[1024];

    /* Physical Locations */
    uintptr_t physical_tables[1024];
    uintptr_t physical;
} __attribute__((packed, aligned(4096))) page_directory_t;

void paging_set_directory(page_directory_t* dir);
void paging_alloc(page_t* page, bool user, bool rw);
void paging_alloc_dma(page_t* page, bool user, bool rw, uintptr_t addr);
uintptr_t page_alloc(uint32_t amount);
void page_free(uint32_t page, uint32_t amount);
page_t* paging_acquire(page_directory_t* dir, uintptr_t addr, bool init);
void paging_release(page_t* page);
page_directory_t* paging_new_directory();
uintptr_t virtual_to_physical(uintptr_t addr);
void paging_install(uintptr_t mem_end, uint32_t mem_size);

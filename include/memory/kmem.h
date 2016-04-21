#define KHEAP_AMT 0x100000

void* kmalloc(unsigned int amnt);
void* kernel_heap_bottom();
void kernel_flush_heap();
void kernel_heap_install(void* kern_end);

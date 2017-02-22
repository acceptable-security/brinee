#include <memory/kmem.h>
#include <stdint.h>

uintptr_t kheap_start;
uintptr_t kheap_current;
uintptr_t kheap_end;
bool kheap_off = false;

uintptr_t _kmalloc(uint32_t size, bool align, uintptr_t* phys) {
    if ( kheap_off ) {
        uintptr_t out;

        if ( align ) {
            out = malloc(size);
        }
        else {
            bsod("Don't have aligned allocations yet.");
        }

        if ( phys ) {
            *phys = virtual_to_physical(out);
        }

        return out;
    }

    if ( align && (kheap_current & 0xFFFFF000) ) {
        kheap_current = (kheap_current & 0xFFFFF000) + 0x1000;
	}

    if ( phys ) {
		*phys = kheap_current;
	}

	uintptr_t address = kheap_current;
	kheap_current += size;
	return address;
}

uintptr_t kmalloc(uint32_t size) {
	return _kmalloc(size, false, NULL);
}

uintptr_t kvmalloc(uint32_t size) {
	return _kmalloc(size, true, NULL);
}

uintptr_t kmalloc_p(uint32_t size, uintptr_t* phys) {
	return _kmalloc(size, false, phys);
}

uintptr_t kvmalloc_p(uint32_t size, uintptr_t *phys) {
	return _kmalloc(size, true, phys);
}

void kernel_flush_heap() {
    memset(kheap_start, 0, (kheap_end - kheap_start));
    kheap_current = kheap_start;
}

uintptr_t kernel_heap_bottom() {
    return kheap_end;
}

void kernel_heap_install(uintptr_t kern_end) {
    kheap_start = kern_end + 1; // + 1 not necessary. just in case.
    kheap_current = kheap_start;
    kheap_end = kheap_start + KHEAP_AMT;
    printf("Kernel heap now at %08x - %08x\n", kheap_start, kheap_end);
    kernel_flush_heap();
}

uintptr_t kernel_heap_switch() {
    kheap_off = true;
    return kheap_end;
}

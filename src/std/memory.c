#include <system.h>
#include <stdlib.h>

#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

void memory_install(void* _end) {
	last_alloc = _end + 0x1000;
	heap_begin = last_alloc;
	pheap_end = 0x400000;
	pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
	heap_end = pheap_begin;
	memset((char *)heap_begin, 0, heap_end - heap_begin);
	pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);
}

void free(void* mem) {
	alloc_t *alloc = (mem - sizeof(alloc_t));
	memory_used -= alloc->size + sizeof(alloc_t);
	alloc->status = 0;
}

void pfree(void* mem) {
	if(mem < pheap_begin || mem > pheap_end)
		return;

	uint32_t ad = (uint32_t)mem;
	ad -= pheap_begin;
	ad /= 4096;

	pheap_desc[ad] = 0;
	return;
}

void* pmalloc(size_t size) {
	int i;

	for(i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++) {
		if(pheap_desc[i])
			continue;

		pheap_desc[i] = 1;

		return (void *)(pheap_begin + i*4096);
	}

	return 0;
}

void* malloc(size_t size) {
	if(!size)
		return 0;

	uint8_t *mem = (uint8_t *)heap_begin;
	while((uint32_t)mem < last_alloc) {
		alloc_t *a = (alloc_t *)mem;

		if(!a->size)
			goto nalloc;

		if(a->status) {
			mem += a->size;
			mem += sizeof(alloc_t);
			mem += 4;
			continue;
		}

		if(a->size >= size) {
			a->status = 1;

			memset(mem + sizeof(alloc_t), 0, size);
			memory_used += size + sizeof(alloc_t);
			return (char *)(mem + sizeof(alloc_t));
		}

		mem += a->size;
		mem += sizeof(alloc_t);
		mem += 4;
	}

	nalloc:;
	if(last_alloc+size+sizeof(alloc_t) >= heap_end) {
		// jesus christ commit slari mari we're done
		// we're out of fucking memory
		// // the mob gonna get mad :(
		// set_task(0);
		// panic("Cannot allocate %d bytes! Out of memory.\n", size);
		puts("Out of memory, halting system.");
		for(;;) {}
		return 0;
	}

	alloc_t *alloc = (alloc_t *)last_alloc;
	alloc->status = 1;
	alloc->size = size;

	last_alloc += size;
	last_alloc += sizeof(alloc_t);
	last_alloc += 4;

	memory_used += size + 4 + sizeof(alloc_t);
	memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);

	return (char *)((uint32_t)alloc + sizeof(alloc_t));
}

void* calloc(size_t num, size_t size) {
	void* ptr = malloc(num * size);
	memset(ptr, 0, num * size);
	return ptr;
}

void *memcpy(void *dest, const void *src, size_t count) {
    const char *sp = (const char *)src;

    char *dp = (char *)dest;
    for(; count != 0; count--) {
        *dp++ = *sp++;
    }

    return dest;
}

void *memset(void *dest, char val, size_t count) {
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count) {
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

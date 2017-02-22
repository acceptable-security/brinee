#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

void free(void* mem);
void pfree(void* mem);
void* pmalloc(size_t size);
void* malloc(size_t size);
void* calloc(size_t num, size_t size);

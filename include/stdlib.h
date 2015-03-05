// STDBOOL
typedef int bool;
#define true 1
#define false 0

// STDINT
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;
typedef signed long int int32_t;
typedef unsigned long int uint32_t;
typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;

// MEM
void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

// STR
size_t strlen(const char *str);
bool strcmp(const char* a, const char* b);

// PORTS
unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, unsigned char _data);

// SUPER MEM
void free(void* mem);
void pfree(void* mem);
void* pmalloc(size_t size);
void* malloc(size_t size);

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

// IO
void cin(char* initial, char* output);
char getchar();
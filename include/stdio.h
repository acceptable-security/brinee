#include <stdarg.h>

extern int printf(const char *format, ...);
extern int sprintf(char *out, const char *format, ...);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern unsigned short inportw (unsigned short _port);
extern void outportw(unsigned short _port, unsigned short _data);
extern unsigned long inportl (unsigned short _port);
extern void outportl (unsigned short _port, unsigned long _data);
extern void cin(char* initial, char* output);
extern char getchar();
extern void puts(char *text);
extern void putch(char c);
extern void cls();

/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Global function declarations and type definitions
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#ifndef __SYSTEM_H
#define __SYSTEM_H

typedef int size_t;

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

/* MAIN.C */
extern void *memcpy(void *dest, const void *src, size_t count);
extern void *memset(void *dest, char val, size_t count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
extern size_t strlen(const char *str);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern unsigned long inportl (unsigned short _port);
extern void outportl (unsigned short _port, unsigned long _data);

/* CONSOLE.C */
extern void init_video(void);
extern void puts(char *text);
extern void putch(char c);
extern void cls();

/* GDT.C */
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

/* IDT.C */

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_install();

/* TIMER.C */
extern void timer_wait(int ticks);
extern void timer_install();

/* KEYBOARD.C */
extern void keyboard_install();

/* MEMORY.C */
extern void memory_install(unsigned int kernel_end);

extern void load_userspace();

#endif

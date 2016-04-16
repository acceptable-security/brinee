#ifndef __SYSTEM_H
#define __SYSTEM_H


/* This defines what the stack looks like after an ISR was running */
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

extern void init_video(void);

extern void gdt_install();
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

extern void isrs_install();

extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);

extern void keyboard_install();

extern void memory_install(void* _end);

extern void load_userspace();

#endif

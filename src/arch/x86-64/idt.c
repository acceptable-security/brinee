#include <kernel/system.h>

/* Defines an IDT entry */
struct idt_entry {
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_entry idt[256];

void idt_empty_entry() {
    __asm__ volatile("iretl");
}

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

inline int idt_enabled() {
    unsigned long flags;
    __asm__ volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

void idt_install() {
    int i;

    struct {
        unsigned short limit;
        unsigned int base;
    } __attribute__((packed)) idtp;

    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;

    for ( i = 0; i < 256; i++ ) {
        idt_set_gate(i, (unsigned) idt_empty_entry, 0x08, 0x8E);
    }

    __asm__ ( "lidt (%0)" : : "g"(&idtp) );
}

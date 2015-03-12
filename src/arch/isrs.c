#include <system.h>
#include <stdio.h>

#define CPU_EXCEPTION(number, message) void isr ## number (struct regs *r) { bsod(#message, r); for(;;) {} }
#define REGISTER_EXCEPTION(number) idt_set_gate(number, (unsigned)isr ## number, 0x08, 0x8E);
/*
 * CPU Exceptions are by nature evil
 * So all we do when they ge thrown
 * Is spit out the error and SHUT EVERYTHING DOWN
 */

void bsod(char* message, struct regs *r) {
    cls();
    printf("PANIC: %s\n\n", message);
    printf("EAX: %X\n", r->eax);
    printf("EBX: %X\n", r->ebx);
    printf("ECX: %X\n", r->ecx);
    printf("EDX: %X\n", r->edx);
    printf("ESi: %X\n", r->esi);
    printf("EDI: %X\n", r->edi);
    printf("\nStack: %X:%X\n", r->ebp, r->esp);
    printf("EIP: %X\n", r->eip);

    for ( ;; ) {}
}

CPU_EXCEPTION(0,  "Dividing by Zero");
CPU_EXCEPTION(1,  "Debug");
CPU_EXCEPTION(2,  "Non Maskable Interrupt");
CPU_EXCEPTION(3,  "Breakpoint");
CPU_EXCEPTION(4,  "Into Detected Overflow");
CPU_EXCEPTION(5,  "Out of Bounds");
CPU_EXCEPTION(6,  "Invalid Opcode");
CPU_EXCEPTION(7,  "No Coprocessor");
CPU_EXCEPTION(8,  "Double Fault");
CPU_EXCEPTION(9,  "Coprocessor Segment Overrun");
CPU_EXCEPTION(10, "Bad TSS");
CPU_EXCEPTION(11, "Segment Not Present");
CPU_EXCEPTION(12, "Stack Fault");
CPU_EXCEPTION(13, "General Protection Fault");
CPU_EXCEPTION(14, "Page Fault");
CPU_EXCEPTION(15, "Uknown Interrupt");
CPU_EXCEPTION(16, "Coprocessor Fault");
CPU_EXCEPTION(17, "Alignment Check");
CPU_EXCEPTION(18, "Machine Check");

void isrs_install() {
    REGISTER_EXCEPTION(0);
    REGISTER_EXCEPTION(1);
    REGISTER_EXCEPTION(2);
    REGISTER_EXCEPTION(3);
    REGISTER_EXCEPTION(4);
    REGISTER_EXCEPTION(5);
    REGISTER_EXCEPTION(6);
    REGISTER_EXCEPTION(7);
    REGISTER_EXCEPTION(8);
    REGISTER_EXCEPTION(9);
    REGISTER_EXCEPTION(10);
    REGISTER_EXCEPTION(11);
    REGISTER_EXCEPTION(12);
    REGISTER_EXCEPTION(13);
    REGISTER_EXCEPTION(14);
    REGISTER_EXCEPTION(15);
    REGISTER_EXCEPTION(16);
    REGISTER_EXCEPTION(17);
    REGISTER_EXCEPTION(18);
}

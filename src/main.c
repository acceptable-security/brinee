#include <kernel/multiboot.h>

#include <drivers/vesa.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern void irq_install();
extern void isrs_install();
extern void idt_install();
extern void gdt_install();
extern void init_video();
extern void memory_install(void* base);
extern void pic_install();
extern void pit_install();
extern void threads_install();
extern void pci_install();
extern void keyboard_install();

extern void* _end; // end of the kernel

// This is the most important part of kernel.
// This is so vital to the computer
// that if you don't have it
// even if you are within a virtual machine
// your entire computer will combust into an inferno of
// hatred and doom. This is literally so important that
// the National Security Agency declared it an enemey of
// the state and has an active campaign against it.

char *flan[4] = {
    "   ______       ",
    "  /      \\     ",
    " /  FLAN  \\    ",
    "/__________\\   "
};



void main(multiboot_info_t* multiboot, unsigned int magic) {
    if ( magic != 0x2BADB002 ) {
        init_video();
        puts("Multiboot Signature Verification Failed!");
        for(;;){}
        return;
    }

    vbe_mode_info_t *vbe_mode_info = (vbe_mode_info_t *) multiboot->vbe_mode_info;

    vbe_install(vbe_mode_info);

    // for ( int x = 100; x < 300; x++ ) {
    //     for ( int y = 100; y < 300; y++ ) {
    //         vbe_putpixel(x, y, 255, 0, 0);
    //     }
    // }

    vbe_putrect(100, 100, 200, 200, 255, 0, 0);
    vbe_putstr(0, 0, "Welcome", 255,255,255,0,0,0);

    for(;;){}
    // INITIALIZE HEAP
    memory_install(&_end);

    // INTIALIZE INTERUPT SERVICES
    init_video();
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    pit_install();
    // ENABLE INTERRUPTS

    __asm__ volatile ("sti");

    printf("Multiboot Magic: 0x%x\n", magic);

    threads_install();

    puts("Uh, this shouldn't be here!");
    for(;;) {}
}

void load_userspace() {
    // INITIALIZE PEREPHERALS
    keyboard_install();
    pci_install();

      // DISPLAY THE FLAN!
    for (int i = 0; i < 4; i++ ) {
        puts(flan[i]);
        putch('\n');
    }

    puts("Welcome to the Brine Kernlee :)) pls hv flan\n");

    while (1) {
        char* output = ( char* ) malloc(128);

        cin("$ ", output);

        if (strcmp(output, "quit") )
            break;
        else if( strcmp(output, "flan") ) {
            puts("i gav u flan lredy :(\ntri agen nxt tiem\n");
        }
        else if( strcmp(output, "break") ) {
            puts("Causing a divide by zero error...\n");
            for ( int i = 0; i < 1; i++ ) {
                i = 2 / i;
            }
        }
        else {
            puts("Got the command ");
            puts(output);
            puts("!\n");
        }

        free(output);
    }

    puts("What? We aint good 'nuff fo ya?\n");
    puts("Well go FUCK YOURSELF\n");
}

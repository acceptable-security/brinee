#include <system.h>
#include <stdlib.h>

extern void pic_install();
extern void pit_install();
extern void threads_install();
extern void pci_install();

extern uint32_t kernel_end; // end of the kernel

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

void main() {
    int i;

    // INITIALIZE HEAP
    memory_install((uint32_t)&kernel_end);

    // INTIALIZE INTERUPT SERVICES
    init_video();
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    pit_install();
    // ENABLE INTERRUPTS

    __asm__ volatile ("sti");

    // load_userspace();

    threads_install();

    puts("Uh, this shouldn't be here!");
    for(;;) {}
}

void load_userspace() {
    int i;
    // INITIALIZE PEREPHERALS
    
    keyboard_install();
    pci_install();

      // DISPLAY THE FLAN!
    for (i = 0; i < 4; i++ ) {
        puts(flan[i]);
        putch('\n');
    }

    puts("Welcome to the Brine Kernlee :)) pls hv flan\n");

    i = 0 / 0;

    while (1) {
        char* output = ( char* ) malloc(128);

        cin("$ ", output);

        if (strcmp(output, "quit") )
            break;
        else if( strcmp(output, "flan") ) {
            puts("i gav u flan lredy :(\ntri agen nxt tiem\n");
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

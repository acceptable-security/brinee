#include <kernel/multiboot.h>
#include <drivers/vesa.h>
#include <tty.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern void irq_install();
extern void isrs_install();
extern void idt_install();
extern void gdt_install();
extern void vga_install();
extern void memory_install(void* base);
extern void pic_install();
extern void pit_install();
extern void threads_install();
extern void pci_install();
extern void keyboard_install();
extern void kernel_heap_install(void* kern_end);

char *flan[4] = {
    "   ______       ",
    "  /      \\     ",
    " /  FLAN  \\    ",
    "/__________\\   "
};

void detect_memory(multiboot_info_t* multiboot) {
    memory_map_t* map = multiboot->mmap_addr;

    printf("== GRUB Memory Map ==\n");
    printf("Address  Length   Type\n");
    while ( map < multiboot->mmap_addr + multiboot->mmap_length ) {
        printf("%08x %08x %s\n", (map->base_addr_high << 4) | map->base_addr_low,
                               (map->length_high << 4) | map->length_low,
                               (map->type == 1 ? "USABLE" : "RESERVED"));
    	map = (memory_map_t*) ((uintptr_t) map + map->size + sizeof(map->size));
	}
}

void use_vesa(multiboot_info_t* multiboot) {
    vbe_mode_info_t *vbe_mode_info = (vbe_mode_info_t *) multiboot->vbe_mode_info;
    vbe_install(vbe_mode_info);
    tty_init(MODE_VESA);
}

void main(multiboot_info_t* multiboot, unsigned int magic, void* _end) {
    if ( magic != 0x2BADB002 ) {
        vga_install();
        puts("Multiboot Signature Verification Failed!");
        for(;;){}
        return;
    }

    vga_install();
    tty_init(MODE_VGA);

    detect_memory(multiboot);

    printf("Address of main: %08x\n", (void*)main);
    printf("End of kernel at %08x\n", _end);
    printf("Multiboot info at %08x\n", multiboot);

    printf("Initializing gdt...\n");
    gdt_install();

    printf("Initializing idt...\n");
    idt_install();

    printf("Initializing frames...\n");
    kernel_heap_install(0x00100000);
    frame_init(0x07ee0000 - 0x00100000);
    printf("Initializing paging...\n");
    paging_install(_end, 0x07ee0000 - 0x00100000); // TODO
    printf("Switching kernel heaps...\n");
    kernel_heap_switch();

    printf("Initializing isrs...\n");
    isrs_install();

    printf("Initializing irq...\n");
    irq_install();

    printf("Initializing pit...\n");
    pit_install();

    printf("Enabling interrupts...\n");

    __asm__ volatile ("sti");

    printf("Multiboot Magic: 0x%x\n", magic);
    printf("Beginning threads...\n");
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

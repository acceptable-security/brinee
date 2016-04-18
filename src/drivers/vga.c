#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned short *textmemptr; // default is VGA mode
int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

void vga_scroll() {
    unsigned blank, temp;

    blank = 0x20 | (attrib << 8);

    // VGA mode has only 25 lines.
    if(csr_y >= 25) {
        temp = csr_y - 25 + 1;

        memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);

        memsetw(textmemptr + (25 - temp) * 80, blank, 80);
        csr_y = 25 - 1;
    }
}

void vga_move_csr() {
    unsigned temp;

    // index = (y * width) + x
    temp = csr_y * 80 + csr_x;

    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

void vga_cls() {
    int i;
    unsigned blank = 0x20 | (attrib << 8);

    for(i = 0; i < 25; i++)
        memsetw(textmemptr + i * 80, blank, 80);

    csr_x = 0;
    csr_y = 0;

    vga_move_csr();
}

void vga_putch(char c) {
    unsigned short *where;
    unsigned att = attrib << 8;

    switch ( c ) {
        case 0x08:
            if(csr_x != 0)
                csr_x--;
            break;

        case 0x09:
            csr_x = (csr_x + 8) & ~(8 - 1);
            break;

        case '\r':
        case '\n':
            csr_x = 0;
            csr_y++;
            break;

        default:
            where = textmemptr + (csr_y * 80 + csr_x);
            *where = c | att;
            csr_x++;
            break;
    }

    if(csr_x >= 80) {
        csr_x = 0;
        csr_y++;
    }

    vga_scroll();
    vga_move_csr();
}

void vga_puts(char *text) {
    int i;

    for (i = 0; i < strlen(text); i++) {
        vga_putch(text[i]);
    }
}

void vga_settextcolor(unsigned char forecolor, unsigned char backcolor) {
    attrib = (backcolor << 4) | (forecolor & 0x0F);
}

void vga_install() {
    textmemptr = (unsigned short *)0xB8000;
    vga_settextcolor(15, 0);
    vga_cls();
}

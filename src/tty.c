#include <tty.h>
#include <drivers/vga.h>
#include <drivers/vesa.h>
#include <string.h>

tty_mode_t tty_mode;
int vesa_x;
int vesa_y;

void putch(char c);

void puts(char* str) {
    switch ( tty_mode ) {
        case MODE_VGA:
            vga_puts(str);
            break;

        case MODE_VESA:
            // Not using putstr, because tty handles newlines differently.

            for ( int i = 0; i < strlen(str); i++ ) {
                putch(((char) str[i]));
            }
            break;
    }
}

void putch(char c) {
    switch ( tty_mode ) {
        case MODE_VGA:
            vga_putch(c);
            break;

        case MODE_VESA:
            if ( c == '\n' ) {
                vesa_x = 15;
                vesa_y += vbe_get_char_height();
                return;
            }

            if ( c == '\t' ) {
                c = ' '; // Temporary hack.
            }

            if ( c == '\b' ) {
                vesa_x -= vbe_get_char_width();
                vbe_putchar(vesa_x, vesa_y, ' ', 255, 255, 255, HEX2RGB(0x496FD6));
                return;
            }

            vbe_putchar(vesa_x, vesa_y, c, 255, 255, 255, HEX2RGB(0x496FD6));
            vesa_x += vbe_get_char_width();

            if ( vesa_x >= vbe_get_screen_width() ) {
                vesa_x = 15;
                vesa_y += vbe_get_char_height();
            }

            break;
    }
}

void cls() {
    switch ( tty_mode ) {
        case MODE_VGA:
            vga_settextcolor(15, 1);
            break;

        case MODE_VESA:
            vbe_putrect(10, 10, 580, 780, HEX2RGB(0x496FD6));
            vbe_putstr(400, 0, "brinee kernel 0.1", 255,255,255,0,0,0);
            vesa_x = 15;
            vesa_y = 15;
            break;
    }
}

void tty_init(tty_mode_t mode) {
    tty_mode = mode;

    switch ( tty_mode ) {
        case MODE_VGA:

            break;

        case MODE_VESA:
            vbe_putrect(10, 10, 580, 780, HEX2RGB(0x496FD6));
            vesa_x = 15;
            vesa_y = 15;
            vbe_putstr(400, 0, "brinee kernel 0.1", 255,255,255,0,0,0);
            break;
    }
}

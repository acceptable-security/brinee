#include <drivers/vesa.h>
#include <drivers/font.h>

unsigned int font_width = 8;
unsigned int font_height = 8;

uint8_t* frame_buffer;
vbe_mode_info_t* vbe_info;

void vbe_putpixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t* ptr = &frame_buffer[(y * vbe_info->pitch) + (x * (vbe_info->bpp / 8))];

    ptr[0] = r & 255;
    ptr[1] = g & 255;
    ptr[2] = b & 255;
}

void vbe_putrect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t* loc = frame_buffer + (y * vbe_info->pitch);
    int bpp = vbe_info->bpp / 8;

    for ( int i = 0; i < w; i++ ) {
        for ( int j = 0; j < h; j++ ) {
            int off = ((j + x) * bpp);
            loc[off + 0] = r;
            loc[off + 1] = g;
            loc[off + 2] = b;
        }

        loc += vbe_info->pitch;
    }
}

void vbe_putchar(int x, int y, char c, uint8_t fr, uint8_t fg, uint8_t fb,
                                       uint8_t br, uint8_t bg, uint8_t bb) {
    int j, h;

    char* font = charset[c - 32];

    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j <= 8; j++ ) {
            if ( font[i] & (1 << (8 - j)) ) {
                vbe_putpixel(j + x, y + i, fr, fg, fb);
            }
            else {
                vbe_putpixel(j + x, y + i, br, bg, bb);
            }
        }
    }
}

void vbe_putstr(int x, int y, const char* str, uint8_t fr, uint8_t fg, uint8_t fb,
                                               uint8_t br, uint8_t bg, uint8_t bb) {
    for ( int i = 0; i < strlen(str); i++ ) {
        vbe_putchar(x + (i * font_width) + (1 * i), y, str[i], fr, fg, fb, br, bg, bb);
    }
}

void vbe_install(vbe_mode_info_t* vbe) {
    frame_buffer = vbe->physbase;
    vbe_info = vbe;
}

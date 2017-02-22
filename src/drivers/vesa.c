#include <drivers/font.h>
#include <drivers/vesa.h>
#include <string.h>

uint8_t* frame_buffer;
vbe_mode_info_t* vbe_info;

void vbe_putpixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t* ptr = frame_buffer + (y * vbe_info->pitch) + (x * (vbe_info->bpp / 8));

    ptr[0] = b & 255;
    ptr[1] = g & 255;
    ptr[2] = r & 255;
}

void vbe_putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t* loc = frame_buffer + (y * vbe_info->pitch);
    int bpp = vbe_info->bpp / 8;

    for ( int i = 0; i < w; i++ ) {
        for ( int j = 0; j < h; j++ ) {
            int off = ((j + x) * bpp);
            loc[off + 0] = b;
            loc[off + 1] = g;
            loc[off + 2] = r;
        }

        loc += vbe_info->pitch;
    }
}

void vbe_putchar(uint32_t x, uint32_t y, uint8_t c, uint8_t fr, uint8_t fg, uint8_t fb,
                                                    uint8_t br, uint8_t bg, uint8_t bb) {
    if ( c < 32 ) {
        return;
    }

    char* font = charset[c - 32];

    for ( int i = 0; i < font_height; i++ ) {
        for ( int j = 1; j <= font_width; j++ ) {
            if ( font[i] & (1 << (font_width - j)) ) {
                vbe_putpixel(j + x, y + i, fr, fg, fb);
            }
            else {
                vbe_putpixel(j + x, y + i, br, bg, bb);
            }
        }
    }
}

void vbe_putstr(uint32_t x, uint32_t y, const char* str, uint8_t fr, uint8_t fg, uint8_t fb,
                                                         uint8_t br, uint8_t bg, uint8_t bb) {
    uint32_t ox = x;
    uint32_t cx = x;

    for ( int i = 0; i < strlen(str); i++ ) {
        if ( str[i] == '\n' ) {
            y += font_height;
            cx = ox;
            continue;
        }

        if ( str[i] == '\t' ) {
            cx += font_width * 4;
            continue;
        }

        vbe_putchar(cx, y, str[i], fr, fg, fb, br, bg, bb);
        cx += font_width;
    }
}

uint32_t vbe_get_char_width() {
    return font_width;
}

uint32_t vbe_get_char_height() {
    return font_height;
}

uint32_t vbe_get_screen_width() {
    return vbe_info->Xres;
}

uint32_t vbe_get_screen_height() {
    return vbe_info->Yres;
}

void vbe_install(vbe_mode_info_t* vbe) {
    frame_buffer = (uint8_t*) vbe->physbase;
    vbe_info = vbe;
}

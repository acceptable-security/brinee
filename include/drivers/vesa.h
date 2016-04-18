#include <kernel/multiboot.h>
#include <stdint.h>

void vbe_putpixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void vbe_putrect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
void vbe_putchar(int x, int y, char c, uint8_t fr, uint8_t fg, uint8_t fb,
                                       uint8_t br, uint8_t bg, uint8_t bb);
void vbe_putstr(int x, int y, const char* str, uint8_t fr, uint8_t fg, uint8_t fb,
                                               uint8_t br, uint8_t bg, uint8_t bb);
void vbe_install(vbe_mode_info_t* vbe);
int vbe_get_char_width();
int vbe_get_char_height();
int vbe_get_screen_width();
int vbe_get_screen_height();

#define HEX2RGB(HEX) ((HEX >> 16) & 0xFF), ((HEX >> 8) & 0xFF), (HEX  & 0xFF)

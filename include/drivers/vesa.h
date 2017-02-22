#include <kernel/multiboot.h>
#include <stdint.h>
#include <color.h>

void vbe_putpixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void vbe_putrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);
void vbe_putchar(uint32_t x, uint32_t y, uint8_t c, uint8_t fr, uint8_t fg, uint8_t fb,
                                                    uint8_t br, uint8_t bg, uint8_t bb);
void vbe_putstr(uint32_t x, uint32_t y, const char* str, uint8_t fr, uint8_t fg, uint8_t fb,
                                                         uint8_t br, uint8_t bg, uint8_t bb);
void vbe_install(vbe_mode_info_t* vbe);
uint32_t vbe_get_char_width();
uint32_t vbe_get_char_height();
uint32_t vbe_get_screen_width();
uint32_t vbe_get_screen_height();

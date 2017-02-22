#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_rgb_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} color_rgba_t;

#define RGB(R, G, B) ((color_rgb_t) { .r = R; .g = G; .b = B })
#define RGBA(R, G, B, A) ((color_rgba_t) { .r = R; .g = G; .b = B, .a = A })

#define HEX2RGB(HEX) ((HEX >> 16) & 0xFF), ((HEX >> 8) & 0xFF), (HEX  & 0xFF)

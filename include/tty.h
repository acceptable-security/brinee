#pragma once

typedef enum {
    MODE_VGA,
    MODE_VESA
} tty_mode_t;

void tty_init(tty_mode_t mode);

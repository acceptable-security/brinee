#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <kernel/system.h>
#include <drivers/ps2.h>

uint8_t ps2_read_status() {
    return inportb(0x64);
}

uint8_t ps2_read_data() {
    while ( ps2_read_status() & ~DATA_READ_BUF_READY );
    return inportb(0x60);
}

void ps2_write_data(uint8_t data) {
    while ( ps2_read_status() & ~DATA_WRITE_BUF_READY );
    outportb(0x60, data);
}

uint8_t ps2_write_command(uint8_t cmd1, uint8_t cmd2, bool resp) {
    outportb(0x64, cmd1);

    if ( cmd2 != 0 ) {
        ps2_write_data(cmd2);
    }

    if ( resp ) {
        return ps2_read_data();
    }

    return 0;
}

void ps2_install() {
    ps2_write_command(0xAD, 0, false); // Disable PS/2 Port 1
    ps2_write_command(0xA7, 0, false); // Disable PS/2 Port 2

    while ( ps2_read_status() & ~DATA_READ_BUF_READY); // Make sure to clear buffers

    uint8_t cfg = ps2_write_command(0x20, 0, true);
    cfg = cfg & ~((1 << 6) | (1 << 1) | (1 << 0));
    ps2_write_command(0x60, cfg, false);

    bool dual_channel = cfg & (1 << 5);

    if ( ps2_write_command(0xAA, 0, false) != 0x55 ) {
        // TODO - PS/2 failed to initialize!
        return;
    }

    if ( dual_channel ) {
        uint8_t cfg2 = ps2_write_command(0xA8, 0, true);

        if ( cfg2 & (1 << 5) ) {
            dual_channel = false;
        }
        else {
            ps2_write_command(0xA7, 0, false);
        }
    }

    if ( ps2_write_command(0xAB, 0, true) != 0x00 ) {
        // TODO - PS/2 self check failed!
        return;
    }

    if ( dual_channel ) {
        if ( ps2_write_command(0xA9, 0, true) != 0x00 ) {
            dual_channel = false;
        }
    }

    ps2_write_command(0xAE, 0, false);
    // ps2_write_command(0xFF, 0, true); // TODO - Reset devices?

    if ( dual_channel ) {
        ps2_write_command(0xA8, 0, false);
    }
}

#include <stdint.h>

typedef enum {
    DATA_READ_BUF_READY  = 1 << 0, // (must be set before attempting to read data from IO port 0x60)
    DATA_WRITE_BUF_READY = 1 << 1, // (must be clear before attempting to write data to IO port 0x60 or IO port 0x64)
    SYSTEM_FLAG          = 1 << 2, // Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST)
    COMMAND_DATA         = 1 << 3, // (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command)
    UNKNOWN_1            = 1 << 4, // May be "keyboard lock" (more likely unused on modern systems)
    UNKNOWN_2            = 1 << 5, // May be "receive time-out" or "second PS/2 port output buffer full"
    TIMEOUT_ERROR        = 1 << 6, // (0 = no error, 1 = time-out error)
    PARITY_ERROR         = 1 << 7  // (0 = no error, 1 = parity error)
} ps2_status_t;

uint8_t ps2_read_status();
uint8_t ps2_read_data();
void ps2_write_data(uint8_t data);
uint8_t ps2_write_command(uint8_t cmd1, uint8_t cmd2, bool resp);
void ps2_install();

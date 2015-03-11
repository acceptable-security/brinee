#include <threads.h> // will include system.h and stdlib.h

struct VBEInfoBlock {
        char sig[4]; // "VESA"
        uint16_t ver; // 0x0300 for 3.0
        uint16_t oemStr[2];
        uint8_t abilities[4];
        uint16_t videoModePtr[2];
        uint16_t totalMem;
};

// this is
// HAAAAAAAAAAAAARD

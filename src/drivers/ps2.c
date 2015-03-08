#include <system.h>
#include <stdlib.h>
#include <kb_us.h>

int keyState = 0; // keeping track of special chars.

typedef struct KeyPress {
    char key;
    bool pressed;
} KeyPress;

KeyPress* keyBuffer;
volatile unsigned int keyBufferLoc = 0;
volatile unsigned int keyBufferReady = 0;

void keyboard_handler() {
    unsigned char scancode = inportb(0x60);

    bool isDown = !(scancode & 0x80);

    int key = (keyState == 0) ? kbdus[scancode & ~0x80] : kbdus_s[scancode & ~0x80];

    if ( key > 0x7F ) { // ASCII Range is 0x7F MAX
        switch ( key ) {
            case 0x81:
                keyState = isDown;
        }
    }
    else {
        if ( keyBuffer != 0) {
            keyBuffer[keyBufferReady].key = (char)key;
            keyBuffer[keyBufferReady].pressed = isDown;
            keyBufferReady++;
            keyBufferReady = keyBufferReady % 1024;
        }
    }
}


char getchar( ) {
    while (true) {
        if(keyBufferLoc == keyBufferReady)
            continue;

        if(!keyBuffer[keyBufferLoc].pressed) {
            keyBufferLoc = (keyBufferLoc + 1) % 1024;
            continue;
        }

        char output = keyBuffer[keyBufferLoc++].key;
        keyBufferLoc = keyBufferLoc % 1024;

        return output;
    }
}

void cin(char* initial, char* output) {
    if (initial)
        puts(initial);

    if ( !output )
        output = (char* ) malloc(128);

    int size = 0;

    while ( size < 128 ) {
        char input = getchar();

        if ( input == '\n' )
            break;

        if ( input == '\b' ) {
            if ( size <= 0 )
                continue;

            size -= 1;
            output[size] = '\0';

            putch('\b');
            putch(' ');
            putch('\b');

            continue;
        }

        output[size] = input;
        putch(output[size]);
        output[size + 1] = '\0';

        size += 1;
    }
    putch('\n');
}

void keyboard_install() {
    keyBuffer = (KeyPress*)calloc(1024, sizeof(KeyPress));
    irq_install_handler(1, keyboard_handler);
}

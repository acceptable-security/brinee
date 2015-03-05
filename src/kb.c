#include <system.h>
#include <stdlib.h>
#include <kb_us.h>

int keyState = 0; // keeping track of special chars.

char* keyboardBuffer;

void keyboard_handler(struct regs *r) {
    unsigned char scancode = inportb(0x60);

    if (scancode & 0x80) {
        int key = (keyState == 0) ? kbdus[scancode & ~0x80] : kbdus_s[scancode & ~0x80];

        if ( key >= 0x80 ) {
            switch ( key ) {
                case 0x81:
                    keyState = 0;

                default:
                    return;
            }
        }

        if ( key == '\b') {
            return;
        }
        else {
            if ( keyboardBuffer != 0) {
                keyboardBuffer[1] = 0; // UP.
            }
        }
    }
    else {
        int key = (keyState == 0) ? kbdus[scancode] : kbdus_s[scancode];

        if ( key > 0x7F ) { // ASCII Range is 0x7F MAX
            switch ( key ) {
                case 0x81:
                    keyState = 1;

                default:
                    return;
            }

            return;
        }

        // pressed
        if ( keyboardBuffer != 0) {
            keyboardBuffer[0] = (char) key;
            keyboardBuffer[1] = 1; // DOWN.
        }
    }
}

char getchar( ) {
    while ( keyboardBuffer[0] == 0 && keyboardBuffer[1] == 0);

    free(keyboardBuffer); // i dont get it. doesnt work without this tho. probably fucks up memory a ton but w/e we'll fix it @ somepoint

    char output = keyboardBuffer[0];

    keyboardBuffer[0] = 0;
    keyboardBuffer[1] = 0;

    return output;
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
    keyboardBuffer = (char* )malloc(2);
    irq_install_handler(1, keyboard_handler);
}

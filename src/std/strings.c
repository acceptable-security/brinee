#include <system.h>
#include <stdlib.h>

size_t strlen(const char *str) {
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

bool strcmp(const char* a, const char* b) {
    if ( !a || !b )
        return false;

    if ( strlen(a) != strlen(b) )
        return false;

    int i;

    for ( i = 0; i < strlen(a); i++ ) {
        if ( a[i] != b[i] )
            return false;
    }

    return true;
}


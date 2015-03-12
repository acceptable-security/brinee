unsigned char inportb (unsigned short _port) {
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (unsigned short _port, unsigned char _data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned long inportl (unsigned short _port) {
    unsigned long rv;
    __asm__ __volatile__ ("inl %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportl (unsigned short _port, unsigned long _data) {
    __asm__ __volatile__ ("outl %1, %0" : : "dN" (_port), "a" (_data));
}

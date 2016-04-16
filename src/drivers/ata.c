#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <drivers/ata.h>

void ata_print_error(int device, unsigned char err) {
    if ( err == 0 )
        return;


}

void ata_read_sector_lba28(int device, int sector_count, int address, char* output) {
    outportb(0x1F1, 0x00);
    outportb(0x1F2, sector_count);
    outportb(0x1F3, (unsigned char) (address >>  0));
    outportb(0x1F4, (unsigned char) (address >>  8));
    outportb(0x1F5, (unsigned char) (address >> 16));
    outportb(0x1F6, 0xE0 | (device << 4) | ((address >> 24) & 0x0F));
    outportb(0x1F7, ATA_CMD_READ_PIO);

    while ( !(inportb(0x1F7) & 0x08) ) {
        // Wait for drive ready.
    }

    for ( int i = 0; i < 256; i++ ) {
        unsigned int tmp = inportw(0x1F0);

        output[i * 2] = (unsigned char) tmp;
        output[i * (2 + 1)] = (unsigned char) (tmp >> 8);
    }
}

void ata_write_sector_lba28(int device, int sector_count, int address, char* input) {
    outportb(0x1F1, 0x00);
    outportb(0x1F2, sector_count);
    outportb(0x1F3, (unsigned char) (address >>  0));
    outportb(0x1F4, (unsigned char) (address >>  8));
    outportb(0x1F5, (unsigned char) (address >> 16));
    outportb(0x1F6, 0xE0 | (device << 4) | ((address >> 24) & 0x0F));
    outportb(0x1F7, ATA_CMD_WRITE_PIO);

    while ( !(inportb(0x1F7) & 0x08) ) {
        // Wait for drive ready.
    }

    for ( int i = 0; i < 256; i++ ) {
        unsigned int tmp = input[8 + (i * 2)] | (input[8 + (i * 2) + 1] << 8);

        outportw(0x1F0, tmp);
    }
}

void ata_read_sector_lba48(int device, int sector_count, long address, char* output) {
    outportb(0x1F1, 0x00);
    outportb(0x1F1, 0x00);

    outportb(0x1F2, (unsigned char) (sector_count >> 8));
    outportb(0x1F2, (unsigned char) (sector_count >> 0));

    outportb(0x1F3, (unsigned char) (address >> 24));
    outportb(0x1F3, (unsigned char) (address >> 0));
    outportb(0x1F4, (unsigned char) (address >> 32));
    outportb(0x1F4, (unsigned char) (address >>  8));
    outportb(0x1F5, (unsigned char) (address >> 40));
    outportb(0x1F5, (unsigned char) (address >> 16));
    outportb(0x1F6, 0x40 | (device << 4));
    outportb(0x1F7, ATA_CMD_READ_PIO_EXT);

    while ( !(inportb(0x1F7) & 0x08) ) {
        // Wait for drive ready.
    }

    for ( int i = 0; i < 256; i++ ) {
        unsigned int tmp = inportw(0x1F0);
        output[i * 2] = (unsigned char) tmp;
        output[i * (2 + 1)] = (unsigned char) (tmp >> 8);
    }
}

void ata_write_sector_lba48(int device, int sector_count, long address, char* input) {
    outportb(0x1F1, 0x00);
    outportb(0x1F1, 0x00);

    outportb(0x1F2, (unsigned char) (sector_count >> 8));
    outportb(0x1F2, (unsigned char) (sector_count >> 0));

    outportb(0x1F3, (unsigned char) (address >> 24));
    outportb(0x1F3, (unsigned char) (address >> 0));
    outportb(0x1F4, (unsigned char) (address >> 32));
    outportb(0x1F4, (unsigned char) (address >>  8));
    outportb(0x1F5, (unsigned char) (address >> 40));
    outportb(0x1F5, (unsigned char) (address >> 16));
    outportb(0x1F6, 0x40 | (device << 4));
    outportb(0x1F7, ATA_CMD_WRITE_PIO_EXT);

    while ( !(inportb(0x1F7) & 0x08) ) {
        // Wait for drive ready.
    }

    for ( int i = 0; i < 256; i++ ) {
        unsigned int tmp = input[8 + (i * 2)] | (input[8 + (i * 2) + 1] << 8);
        outportw(0x1F0, tmp);
    }
}

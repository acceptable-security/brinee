ARCH = i386
ASM = nasm
CC = gcc

DEFAULT_CFLAGS = -Wall -O -fstrength-reduce -fomit-frame-pointer -ffreestanding  -finline-functions -c -g -std=c11

ASMFLAGS = -f elf -g
CFLAGS = $(DEFAULT_CFLAGS) -m32 -Iinclude/ -Iinclude/arch/
LFLAGS = -m32 -ffreestanding -O2 -nostdlib -g

C_SOURCES = $(wildcard src/*/*.c) $(wildcard src/*.c) $(wildcard src/arch/*/*.c)
C_OBJECTS = $(C_SOURCES:.c=.o)

ASM_SOURCES = $(wildcard src/arch/core/*.asm)
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)

kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	$(CC) $(LFLAGS) -T src/arch/link.ld -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS)


$(C_OBJECTS):
	$(CC) $(CFLAGS) $(subst .o,.c,$@) -o $@

$(ASM_OBJECTS):
	$(ASM) $(ASMFLAGS) $(subst .o,.asm,$@) -o $@

clean:
	rm -f $(C_OBJECTS) $(ASM_OBJECTS) kernel.bin brinee.iso
	rm -rf isodir

image:
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o brinee.iso isodir

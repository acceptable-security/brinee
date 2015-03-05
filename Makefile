ASM = nasm
ASMFLAGS = -f elf

CC = cc
CFLAGS = -Wall -O -m32 -fstrength-reduce -fomit-frame-pointer -ffreestanding  -finline-functions -nostdinc -fno-builtin -Iinclude/ -c

LFLAGS = -m32 -ffreestanding -O2 -nostdlib -lgcc

C_SOURCES = $(strip $(wildcard src/*.c))
C_OBJECTS = $(subst src/, ./, $(C_SOURCES:.c=.o))

ASM_SOURCES = $(strip $(wildcard src/core/*.asm))
ASM_OBJECTS = $(subst src/core/,./,$(ASM_SOURCES:.asm=_internal.o))

kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	gcc $(LFLAGS) -T link.ld -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS)

$(C_OBJECTS):
	$(CC) $(CFLAGS) $(subst .o,.c, src/$@) -o $@

$(ASM_OBJECTS):
	$(ASM) $(ASMFLAGS) $(subst .o,,$(subst _internal,,src/core/$@)).asm -o $@

clean:
	rm -f *.o kernel.bin

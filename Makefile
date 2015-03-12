ASM = nasm
ASMFLAGS = -f elf

CC = cc
CFLAGS = -Wall -O -m32 -fstrength-reduce -fomit-frame-pointer -ffreestanding  -finline-functions -Iinclude/ -c -g

LFLAGS = -m32 -ffreestanding -O2 -nostdlib -lgcc -g

C_SOURCES = $(wildcard src/*/*.c) $(wildcard src/*.c)
C_OBJECTS = $(C_SOURCES:.c=.o)

ASM_SOURCES = $(wildcard src/core/*.asm)
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)

kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	gcc $(LFLAGS) -T link.ld -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS)


$(C_OBJECTS):
	$(CC) $(CFLAGS) $(subst .o,.c,$@) -o $@

$(ASM_OBJECTS):
	$(ASM) $(ASMFLAGS) $(subst .o,.asm,$@) -o $@

clean:
	rm -f $(C_OBJECTS) $(ASM_OBJECTS) kernel.bin

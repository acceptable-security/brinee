
ifeq ($(arch),)
	ARCH = i386
else
	ARCH = $(arch)
endif

ASM = nasm
CC = cc

DEFAULT_CFLAGS = -Wall -O -fstrength-reduce -fomit-frame-pointer -ffreestanding  -finline-functions -c -g

ifeq ($(ARCH), i386)
	ASMFLAGS = -f elf
	CFLAGS = $(DEFAULT_CFLAGS) -m32 -Iinclude/ -Iinclude/arch/$(ARCH)/
	LFLAGS = -m32 -ffreestanding -O2 -nostdlib -lgcc -g
else ifeq ($(ARCH), x86-64)
	ASMFLAGS = -f elf64
	CFLAGS = $(DEFAULT_CFLAGS) -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -Iinclude/ -Iinclude/arch/$(ARCH)/
	LFLAGS = -ffreestanding -O2 -nostdlib -lgcc -g
endif

C_SOURCES = $(wildcard src/*/*.c) $(wildcard src/*.c) $(wildcard src/arch/$(ARCH)/*.c) $(wildcard src/arch/$(ARCH)/*/*.c)
C_OBJECTS = $(C_SOURCES:.c=.o)

ASM_SOURCES = $(wildcard src/arch/$(ARCH)/core/*.asm)
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)

kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	gcc $(LFLAGS) -T src/arch/$(ARCH)/link.ld -o kernel.bin $(ASM_OBJECTS) $(C_OBJECTS)


$(C_OBJECTS):
	$(CC) $(CFLAGS) $(subst .o,.c,$@) -o $@

$(ASM_OBJECTS):
	$(ASM) $(ASMFLAGS) $(subst .o,.asm,$@) -o $@

clean:
	rm -f $(C_OBJECTS) $(ASM_OBJECTS) kernel.bin

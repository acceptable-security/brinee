[BITS 32]
global start
start:
    mov esp, sys_stack
    jmp stublet

ALIGN 4

mboot:
    ; Multiboot macros to make a few lines later more readable

    MULTIBOOT_VESA_MODE     equ 0 ;;linear
    MULTIBOOT_VESA_WIDTH    equ 800
    MULTIBOOT_VESA_HEIGHT   equ 600
    MULTIBOOT_VESA_DEPTH    equ 32 ;;24

    MULTIBOOT_PAGE_ALIGN	equ 1<<0
    MULTIBOOT_MEMORY_INFO	equ 1<<1
    MULTIBOOT_VIDEO_MODE    equ 1<<2
    MULTIBOOT_AOUT_KLUDGE	equ 1<<16
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE ;| MULTIBOOT_VIDEO_MODE
    MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    EXTERN code, bss, _end

    ; This is the GRUB Multiboot header. A boot signature
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

    dd mboot
    dd code
    dd bss
    dd _end
    dd start

    dd MULTIBOOT_VESA_MODE
    dd MULTIBOOT_VESA_WIDTH
    dd MULTIBOOT_VESA_HEIGHT
    dd MULTIBOOT_VESA_DEPTH

stublet:
    push _end
    push eax
    push ebx

    extern main
    call main
    jmp $

SECTION .bss
    resb 8192               ; This reserves 8KBytes of memory here
sys_stack:

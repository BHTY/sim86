; BIOS entry point

[org 0x0]

entry_point:
	mov sp, 0x7bfe
	mov ax, 0
	mov ss, ax
	mov ax, cs
	mov ds, ax
	call copy_ivt
	sti
	call print_welcome_message
	call memory_test
	call set_bda
	int 0x19
inf_loop: hlt

print_welcome_message:
	mov bx, welcome_message
	call print_string
	ret
	
memory_test: ; the memory test will go through memory, first writing over every byte with 0xAA, seeing if it's right, and then 0x00 and seeing if it's right
	mov bx, ok_msg
	call print_string
	ret

set_bda:
	push ds
	push 0x40
	pop ds
	mov ax, 0x21
	mov ds:[0x10], ax
	mov ax, 0x280
	mov ds:[0x13], ax
	pop ds
	ret
	
welcome_message: db "sim86 PC BIOS Version 0.01", 10, "(c) Will Klees 2023", 10, 0
ok_msg: db "640K OK", 0
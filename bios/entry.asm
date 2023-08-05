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
	int 0x19
inf_loop: hlt

print_welcome_message:
	mov bx, welcome_message
	call print_string
	ret
	
memory_test:
	mov bx, ok_msg
	call print_string
	ret
	
welcome_message: db "sim86 PC BIOS by Will Klees", 10, "Version 0.01", 10, 0
ok_msg: db "640K OK", 0
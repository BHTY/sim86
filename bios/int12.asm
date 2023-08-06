; int 12h bios services

int12_isr:
	push ds
	push 0x40
	pop ds
	mov ax, ds:[0x13]
	pop ds
	iret
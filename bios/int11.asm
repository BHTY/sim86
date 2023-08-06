; int 11h services

int11_isr:
	push ds
	push 0x40
	pop ds
	mov ax, ds:[0x10]
	pop ds
	iret
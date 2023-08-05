; int 18h services
; this is just a stub to prevent ROM BASIC from being loaded by IBM BASICA

int18_isr:
	push ds
	push bx
	mov bx, cs
	mov ds, bx
	mov bx, rom_basic_msg
	call print_string
	pop bx
	pop ds
	iret
	
rom_basic_msg: db "No ROM BASIC! Insert boot disk and press CTRL-ALT-DEL", 0
; int 19h services

; loads the boot sector of device 0 into 0000:7C00
int19_isr:
	xor ax, ax
	mov es, ax
	mov bx, 0x7c00
	mov ah, 0x02
	mov al, 0x01
	mov ch, 0x0
	mov cl, 0x01
	mov dh, 0x0
	mov dl, 0x0
	int 0x13
	mov bx, 0x7dfe
	push ds
	mov ax, 0
	mov ds, ax
	cmp word [bx], 0xaa55 ;check for valid boot sector
	jne exec_basic
;print loading message
	push ds
	push bx
	push cs
	pop ds
	mov bx, loading_msg
	call print_string
	pop bx
	pop ds
	jmp 0x0000:0x7c00
exec_basic:
	pop ds
	int 0x18

loading_msg: db "Loading disk...", 0
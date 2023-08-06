; int 16h keyboard services

int16_isr:
	push ds
	push bx
	push BDA_SEGMENT
	pop ds
	xor bx, bx
	mov bl, ah
	shl bx, 1
	mov bx, [cs:int16_jump_table + bx]
	call bx
	pop bx
	pop ds
	iret
	
int16_read_keypress:
wait_loop:
	mov al, [ds:NUM_KEYS]
	cmp al, 0x00
	je wait_loop
;it's not 0, so we pop a key
	dec byte [ds:NUM_KEYS]
	call read_key_top
	ret

int16_get_kbdstate: ;peek a key from the top of the buffer (AX=0 if nothing is there)
	mov al, [ds:NUM_KEYS]
	cmp al, 0x00
	je kbdstate_ret0
	call read_key_top
	ret
kbdstate_ret0:
	xor ax, ax
	ret
	
; store current key index in AL, return scancode & ascii code in AX
read_key_top:
	xor bx, bx
	mov bl, al
	mov ah, [ds:bx+START_KEYBUF]
	xor bx, bx
	mov bl, ah
	mov al, [cs:key_scancode_table + bx]
	ret
		
key_isr:
;initialization
	cli
	push ds
	push bx
	push ax
	push BDA_SEGMENT
	pop ds
;read from the keyboard controller
	in al, 0x60
	cmp al, 0x80 ; is this a release code?
	jae key_isr_done
;append to the list of pushed keys
	mov bx, [ds:NUM_KEYS]
	cmp bl, 32 ;ensure that the list isn't full
	jae key_isr_done
;otherwise, write the result back
	mov [ds:bx+START_KEYBUF], al
	inc byte [ds:NUM_KEYS]
key_isr_done:
	pop ax
	pop bx
	pop ds
	sti
	iret
	
BDA_SEGMENT equ 0x0040
START_KEYBUF equ 0x0020
NUM_KEYS equ START_KEYBUF + 0x0020

int16_jump_table:
	dw int16_read_keypress
	dw int16_get_kbdstate
	
key_scancode_table:
	
; The keyboard has two modes of operation, make/release and typematic (though they can be engaged at the same time, as is their default operation mode - this BIOS only cares about typematic)
; In make/release mode, whenever a key is pressed down, a byte is sent indicating that to the computer. Similarly, a different code identifying the key released is sent when a key is released.
; In typematic mode, if a key is held down, its code will be repeatedly transmitted (at a specified repeat rate which can be controlled).
; When in hybrid mode, if a key is held down, its scancode will be repeatedly sent, but then another code will be sent when it is released.

; Whenever the keyboard controller has a byte to send (for example, whenever the typematic delay has elapsed and a keycode is resent), it will signal interrupt 9 on the CPU.
; The int9 handler (key_isr) immediately reads a byte from port 0x60 (the keyboard controller's I/O port).
; If the MSB of the bit is set, that's a release signal, which is ignored.
; However, if the MSB is unset, the lower 7 bits of the byte indicate the key's scancode. 
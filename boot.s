[org 0x7c00]
mov bx, string
call put_string

int 0x18
jmp $

print_char: ;character passed in al
	mov ah, 0x0e
	int 0x10
	ret

put_string:
print_loop:
	mov al, [bx]
	cmp al, 0
	je exit_print
	call print_char
	inc bx
	jmp print_loop
exit_print:
	ret

string: db 10, "We have booted from a floppy disk, but we're about to call the interrupt to enter BASIC.", 10, 0

times 510-($-$$) db 0
db 0x55, 0xaa
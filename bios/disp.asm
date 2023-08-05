; common text printout services

print_char: ;character passed in al
	mov ah, 0x0e
	int 0x10
	ret

print_string: ;pointer to string passed in bx
print_loop:
	mov al, [bx]
	cmp al, 0
	je exit_print
	call print_char
	inc bx
	jmp print_loop
exit_print:
	ret

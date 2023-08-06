; int 13h services

int13_isr:
	;cli
	push ds
	push bx
	mov bx, cs
	mov ds, bx
	xor bx, bx
	mov bl, ah
	shl bx, 1
	mov bx, [int13_jump_table + bx]
	;hlt
	call bx
	pop bx
	;sti
	pop ds
	iret
	
int13_reset:
	ret
	
int13_read:
	mov bp, sp
	mov bx, [bp + 0x2]
	mov si, 0
int13_read_loop:
	call int13_read_sector
	add bx, 0x200
	inc cl
	inc si
	dec al
	cmp al, 0x00
	je int13_read_done
	jmp int13_read_loop
int13_read_done:	
	xor ah, ah
	mov ax, si
	ret
	
int13_read_sector:
	push dx
	push cx
	push ax
	call int13_sel_dev
	mov al, 0x1
	out 0x2, al
	mov dx, 0x3
	mov di, bx
	mov cx, 0x100
	rep insw
	pop ax
	pop cx
	pop dx
	ret
	
; devnum in DL, track in CH, sector in CL, head in DH
int13_sel_dev:
	mov al, 0x3
	out 0x2, al
	mov al, dl
	out 0x3, al
	mov al, 0x00
	out 0x2, al
; output track #
	mov al, ch
	out 0x3, al
; output head #
	mov al, dh
	out 0x3, al
; output sector #
	mov al, cl
	out 0x3, al
	ret
	
int13_get_params: ;drive # stored in DL, returns # of disks
	;this is a literal stub
	xor bl, bl
	xor dl, dl
	ret
	
int13_jump_table:
	dw int13_reset
	dw 0 ;status
	dw int13_read
	dw 0 ;write
	dw 0 ;verify
	dw 0 ;format
	dw 0 ;
	dw 0 ;
	dw int13_get_params ;get drive params
	
; The Disk Interface works via two ports - a command port (0x02) and a data port (0x03)
; The command port and accepts the following commands:
;      Write 0x00: Seek Sector
;			The value written to the data port is the sector that will be seeked to.
;			Pass track/cylinder, then sector, then head
;      Write 0x01: Read Sector
;			Read 256 16-bit values from the data port (each read advances the index in the current sector)
;      Write 0x02: Write Sector
;			Write 256 16-bit values to the data port (each write advances the index in the current sector)
;	   Write 0x03: Switch Device
;			Write the device ID to the data port to select the new device number

; A standard 160K floppy disk has 40 tracks each of which have 8 sectors
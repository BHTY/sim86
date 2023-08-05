; IVT-related copy
copy_ivt:
	mov ax, 0
	mov es, ax
	mov di, 0
	mov si, IVT
	mov cx, 0x400
	rep movsb
	ret

IVT:
;int 00
	dw 0
	dw 0
;int 01
	dw 0
	dw 0
;int 02
	dw 0
	dw 0
;int 03
	dw 0
	dw 0
;int 04
	dw 0
	dw 0
;int 05
	dw 0
	dw 0
;int 06
	dw 0
	dw 0
;int 07
	dw 0
	dw 0
;int 08
	dw 0
	dw 0
;int 09
	dw 0
	dw 0
;int 0a
	dw 0
	dw 0
;int 0b
	dw 0
	dw 0
;int 0c
	dw 0
	dw 0
;int 0d
	dw 0
	dw 0
;int 0e
	dw 0
	dw 0
;int 0f
	dw 0
	dw 0
;int 10
	dw int10_isr
	dw 0xf000
;int 11
	dw 0
	dw 0
;int 12
	dw 0
	dw 0
;int 13
	dw int13_isr
	dw 0xf000
;int 14
	dw 0
	dw 0
;int 15
	dw 0
	dw 0
;int 16
	dw 0
	dw 0
;int 17
	dw 0
	dw 0
;int 18
	dw int18_isr
	dw 0xf000
;int 19
	dw int19_isr
	dw 0xf000


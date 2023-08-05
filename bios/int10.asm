; int 10h video bios services

int10_isr:
	out 0x0, al
	iret
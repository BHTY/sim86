; combined bios

%include "entry.asm"
%include "int10.asm"
%include "int13.asm"
%include "int18.asm"
%include "int19.asm"
%include "ivt.asm"
%include "disp.asm"

times 65520-($-$$) db 0
%include "boot.asm"
times 16-5 db 0
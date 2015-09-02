SEGMENT .text
 xor eax, eax
 mov al, 0x5
 xor ebx, ebx
 mov bl, 0x5
 xor ecx, ecx 
 mov cl, 0x5
 ;mov dx, 0x5
 ;mov si, 0x5
 vmcall

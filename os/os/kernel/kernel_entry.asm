; prepended in front of the actual kernel.o to ensure that this is always executed first and that we always enter the kernel at main()
[bits 32]
[extern main]

call main

jmp $	; halt?
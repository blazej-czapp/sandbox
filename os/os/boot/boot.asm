; A boot sector that enters 32 - bit protected mode.
[org 0x7c00]

KERNEL_OFFSET equ 0x1000				; equ doesn't allocate any memory, just defines a compiler
										; constant

mov [BOOT_DRIVE], dl 					; from BIOS

mov bp, 0x9000							; Set the stack.
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string

call load_kernel						; load the kernel image while we still have BIOS to do
										; disk access for us

call switch_to_pm						; Note that we never return from here.

jmp $

%include "boot/print_string.asm"
%include "boot/disk_load.asm"
%include "boot/gdt.asm"
%include "boot/print_string_pm.asm"
%include "boot/switch_to_pm.asm"

[bits 16]
load_kernel :
	mov bx, MSG_LOAD_KERNEL
	call print_string

	; Setup parameters for our disk_load routine, so
	; that we load the first 15 sectors (excluding
	; the boot sector) from the boot disk (i.e. our
	; kernel code) to address KERNEL_OFFSET
	mov bx, KERNEL_OFFSET
	mov dh, 25
	mov dl, [BOOT_DRIVE]

	call disk_load
	ret

[bits 32]

BEGIN_PM:
	mov ebx, MSG_PROT_MODE
	call print_string_pm				; Use our 32-bit print routine

	call KERNEL_OFFSET					; we don't expect to return from here

	jmp $


; Globals
MSG_REAL_MODE	db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE	db "PM!", 0 ;"Successfully landed in 32-bit Protected Mode", 0
MSG_LOAD_KERNEL db "Loading the kernel image", 0
CALLING_DISK_LOAD db "Calling disk load"

BOOT_DRIVE		db 0

; Bootsector padding
times 510-($-$$)	db 0
dw 0xaa55
[bits 16]

; prints string pointed to by bx
print_string:
	pusha
	mov ah, 0x0e	; initialise BIOS telex mode

.next_char:
	mov cx, [bx]
	cmp cl, 0
	je .end

	mov al, cl		; current character
	int 0x10
	add bx, 1
	jmp .next_char

.end:
	popa
	ret

; --------------------------------------------

; prints (in hex) the address stored in dx
; uses memory region under HEX_OUT as a template
print_hex:
	pusha

	mov bx, HEX_OUT
	add bx, 2		; skip the initial '0x'

	mov ax, dx
	shr ax, 12	 	; 4th character from right
	call prepare
	add bx, 1

	mov ax, dx
	and ax, 0x0f00
	shr ax, 8	 	; 3rd character from right
	call prepare
	add bx, 1

	mov ax, dx
	and ax, 0x00f0
	shr ax, 4	 	; 2nd character from right
	call prepare
	add bx, 1

	mov ax, dx
	and ax, 0x000f  ; 1st character from right
	call prepare

	mov bx, HEX_OUT		; print the string pointed to by BX
	call print_string
	popa
	ret

; sets the char in HEX_OUT pointed to by bx to ASCII representation of the value at al
prepare:
	pusha
	mov cx, 0x30	; zero is ASCII 0x30
	cmp al, 0xa		; is number?
	jl .apply 		; no extra offset necessary
	mov cx, 0x61	; 0x61 is ASCII for 'a', need to subtract 10 from that to account for a being 10
	sub cx, 0xa
	jmp .apply

.apply:
	add al, cl		; ASCII for the character we want to print
	mov [bx], al

	popa
	ret

; global variables
	HEX_OUT: db '0x0000', 0
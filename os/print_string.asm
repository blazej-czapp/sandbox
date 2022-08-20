print_string:
	pusha
	mov ah, 0x0e	; initialise telex mode

next_char:
	mov cx, [bx]	; current character
	cmp cl, 0
	je finish
	mov al, cl
	int 0x10
	add bx, 1
	jmp next_char

finish:
	popa
	ret
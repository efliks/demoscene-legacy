; ds:si - ptr to filename
; ds:di - ptr to array
; cx - size

load_file proc
	mov	ax, 3d00h
	mov	dx, si
	int	21h
	jc	file_error

	mov	bx, ax
	mov	ax, 3f00h
	mov	dx, di
	int	21h
	jc	file_error

	mov	ax, 3e00h
	int	21h
	jc	file_error

	mov	ax, 1
	ret
file_error:
	xor	ax, ax
	ret
endp
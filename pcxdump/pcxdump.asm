
;**********************************************************************
;* SAVE_PCX ROUTINE (03.05.2004)                                      *
;* Coded by Majuma under NASM                                         *
;*                                                                    *
;*  [ Majuma ............... majuma@wp.pl GG:1005762 ]                *
;*  [ Majuma's Home Page ... http://www.majuma.xt.pl ]                *
;*                                                                    *
;* All right reserved.                                                *
;**********************************************************************

bits 16

PCX_HEADER_SIZE equ 128
FILE_BUFFER_SIZE equ 2048

;**********************************************************************

pcx_header:
db 10, 5, 1, 8, 0, 0, 0, 0, 63, 1, 199, 0, 64, 1, 200
db 0, 254, 36, 242, 191, 119, 115, 70, 0, 0, 0, 0, 0, 118, 7, 0, 0, 124, 6, 0, 0, 144
db 244, 124, 0, 172, 240, 124, 0, 24, 0, 0, 0, 197, 185, 247, 191, 24, 0, 0, 0, 0, 0
db 0, 0, 2, 0, 0, 0, 1, 1, 64, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

filename	db 'scr00000.pcx', 0
txt_buffer	db '00000000', 0

counter		dw 0
freq_temp	dw 0
pcx_error_code	dw 0
buffer_ptr	dw 0
put_char	db 0
curr_byte	db 0

;**********************************************************************

; PROCEDURE SAVE_PCX
; input: es = segment containing 320x200x256 screen,
; ax = frequency (ex. if ax = 2 then every second screen will be saved)
save_pcx:
	pusha
	push	ds

	push	cs
	pop	ds

	mov	bx, word [freq_temp]
	inc	bx
	cmp	bx, ax
	jae	sp_oooo
	mov	word [freq_temp], bx
	jmp	sp_quit
sp_oooo:
	mov	word [freq_temp], 0

	; check counter
	mov	ax, word [counter]
	cmp	ax, 10000
	jb	sp_oki
	jmp	sp_quit
sp_oki:

	; make place for file buffer
	sub	sp, FILE_BUFFER_SIZE
	mov	bp, sp

	; make filename
	mov	ax, word [counter]
	mov	di, txt_buffer
	xor	cx, cx
	mov	bx, 10
sp_do1:
	xor	dx, dx
	div	bx
	push	dx
	inc	cx
	or	ax, ax
	jnz	sp_do1
sp_do2:
	pop	ax
	add	al, '0'
	mov	byte [di], al
	inc	di
	loop	sp_do2
	xor	al, al
	mov	byte [di], al

	xor	cx, cx
	mov	si, txt_buffer
	cld
sp_cnt:
	lodsb
	or	al, al
	jz	sp_cntquit
	inc	cx
	jmp	sp_cnt
sp_cntquit:

	push	cx
	neg	cx
	add	cx, 5
	mov	dx, cx
	mov	si, filename + 3
	mov	al, '0'
sp_putzero:
	mov	byte [si], al
	inc	si
	loop	sp_putzero

	pop	cx
	mov	si, txt_buffer
	mov	di, filename + 3
	add	di, dx
sp_putnum:
	lodsb
	mov	byte [di], al
	inc	di
	loop	sp_putnum

	; make file
	mov	ah, 0x3c
	mov	dx, filename
	xor	cx, cx
	int	0x21
	jnc	sp_fileok
	mov	word [pcx_error_code], ax
	xor	ax, ax
	ret
sp_fileok:
	mov	bx, ax

	; write header
	mov	ah, 0x40
	mov	cx, PCX_HEADER_SIZE
	mov	dx, pcx_header
	int	0x21

	; write image
	xor	ax, ax
	mov	word [buffer_ptr], ax

	xor	di, di
sp_writeimg:
	mov	al, byte [es:di]
	mov	byte [curr_byte], al

	xor	dx, dx
sp_x1:
	mov	ax, dx
	add	ax, di
	cmp	ax, 64000
	jae	sp_x1break
	mov	al, byte [curr_byte]
	push	bx
	mov	bx, dx
	cmp	byte [es:di + bx], al
	pop	bx
	jne	sp_x1break
	inc	dx
	cmp	dx, 63
	jne	sp_x1
sp_x1break:

	;mov	al, byte [curr_byte]
	;cmp	al, 192
	;jbe	sp_okxxx
	;mov	byte [curr_byte], 192
sp_okxxx:

	or	dx, dx
	jnz	sp_okyyy

	;mov	ah, 0x40
	;mov	cx, 1
	;mov	dx, curr_byte
	;int	0x21
	mov	al, byte [curr_byte]
	call	_save_in_buffer
	
	inc	di
	jmp	sp_next
sp_okyyy:
	push	dx
	mov	al, byte [curr_byte]
	push	ax
	mov	al, dl
	or	al, 192
	;mov	byte [curr_byte], al
	;mov	dx, curr_byte
	;mov	cx, 1
	;mov	ah, 0x40
	;int	0x21
	call	_save_in_buffer

	pop	ax
	;mov	byte [curr_byte], al
	;mov	ah, 0x40
	;int	0x21
	call	_save_in_buffer

	pop	ax
	add	di, ax 
sp_next:

	cmp	di, 64000
	jne	sp_writeimg

	call	_empty_buffer

	; get palette and convert it
	mov	dx, 0x03c7
	xor	al, al
	out	dx, al
	add	dx, 2
	xor	si, si
	mov	cx, 768
sp_getpal:
	in	al, dx
	shl	al, 2
	mov	byte [ss:bp + si], al
	inc	si
	loop	sp_getpal

	; write palette
	push	ds
	mov	ax, ss
	mov	ds, ax
	mov	dx, bp
	mov	cx, 768
	mov	ah, 0x40
	int	0x21
	pop	ds

	; close file
	mov	ah, 0x3e
	int	0x21

	; update counter
	inc	word [counter]

	; free place
	add	sp, FILE_BUFFER_SIZE

sp_quit:
	pop	ds
	popa
	ret

; PROCEDURE _SAVE_IN_BUFFER
; in: al = byte to save
_save_in_buffer:
	push	si
	mov	si, word [buffer_ptr]
	mov	byte [ss:bp + si], al
	inc	si
	cmp	si, FILE_BUFFER_SIZE
	je	__sib_dump
	mov	word [buffer_ptr], si
	pop	si
	ret
__sib_dump:
	pop	si

; PROCEDURE _EMPTY_BUFFER
; in: none
_empty_buffer:
	push	dx
	push	ds
	push	ax
	push	cx
	mov	cx, word [buffer_ptr]
	inc	cx
	jcxz	__eb_exit
	mov	ax, ss
	mov	ds, ax
	mov	dx, bp
	mov	ah, 0x40
	int	0x21
__eb_exit:
	pop	cx
	pop	ax
	pop	ds
	pop	dx
	mov	word [buffer_ptr], 0
	ret

;**********************************************************************

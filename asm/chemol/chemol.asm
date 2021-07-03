
;**********************************************************************
;* Chemol v.1.0 (27.03.2004)                                          *
;* Coded by Majuma under NASM                                         *
;*                                                                    *
;*  [ Majuma ............... majuma@wp.pl ... GG:1005762 ]            *
;*  [ Majuma's Home Page ... http://republika.pl/majuma/ ]            *
;*                                                                    *
;* All right reserved.                                                *
;**********************************************************************

bits 16
org 0x100

;----------------------------------------------------------------------

MAX_DEGS	equ 1024

IMAGE_W		equ 256
IMAGE_H		equ 128
IMAGE_SIZE	equ IMAGE_W * IMAGE_H

IMAGE_PTR	equ 0
PAL_PTR		equ IMAGE_PTR + IMAGE_SIZE

WINDOW_W	equ 320
WINDOW_H	equ 140
WINDOW_SIZE	equ WINDOW_W * WINDOW_H

L_SINX		equ 0
L_COSX		equ L_SINX + WINDOW_W * 4
L_SINY		equ L_COSX + WINDOW_W * 4
L_COSY		equ L_SINY + WINDOW_H * 4

L_SINTAB	equ L_COSY + WINDOW_H * 4
L_COSTAB	equ L_SINTAB + MAX_DEGS * 2

SCROLL_PTR	equ L_COSTAB + MAX_DEGS * 2

SCROLL_WAIT	equ 400

;----------------------------------------------------------------------

	push	cs
	pop	ds

	cld

	; alloc memory
	call	alloc_seg
	or	ax, ax
	jnz	memok1
	jmp	out_of_mem1
memok1:
	mov	word [scr_buffer], ax

	call	alloc_seg
	or	ax, ax
	jnz	memok2
	jmp	out_of_mem2
memok2:
	mov	word [the_mem], ax

	; load files
	mov	si, f_image
	mov	es, ax
	mov	di, IMAGE_PTR
	call	load_file
	or	ax, ax
	jnz	fileok1
	jmp	file_error
fileok1:
	mov	si, f_palette
	mov	di, PAL_PTR
	call	load_file
	or	ax, ax
	jnz	fileok2
	jmp	file_error
fileok2:

	; generate sinus and cosinus lookups
	xor	bx, bx
	mov	cx, MAX_DEGS
	finit
	fldz
genlookups:
	fld	st0
	fsin
	fmul	dword [muler]
	fistp	word [_data + L_SINTAB + bx]
	fld	st0
	fcos
	fmul	dword [muler]
	fistp	word [_data + L_COSTAB + bx]
	fadd	dword [angle_add]
	add	bx, 2
	loop	genlookups
	ffree	st0

	; find the brightest color in palette and set font_color
	push	ds
	mov	ax, word [the_mem]
	mov	ds, ax
	mov	si, PAL_PTR
	xor	bp, bp
	xor	di, di
	xor	dx, dx
find_col:
	push	dx
	lodsb
	xor	bx, bx
	mov	bl, al
	lodsb
	xor	dx, dx
	mov	dl, al
	lodsb
	xor	ah, ah
	add	ax, bx
	add	ax, dx
	xor	dx, dx
	mov	bx, 3
	div	bx
	pop	dx
	cmp	ax, dx
	jbe	find_col_skip
	mov	dx, ax
	mov	di, bp
find_col_skip:
	inc	bp
	cmp	bp, 256
	jne	find_col
	pop	ds
	mov	ax, di
	mov	byte [font_color], al

	; clear buffers
	mov	ax, word [scr_buffer]
	mov	es, ax
	xor	di, di
	xor	eax, eax
	mov	cx, 64000 / 4
	rep	stosd
	push	ds
	pop	es
	mov	di, _data + SCROLL_PTR
	mov	cx, 320 * 8 / 4
	rep	stosd

	; set 320x200x256 VGA mode
	mov	ax, 0x13
	int	0x10

	; set palette & fix image
	mov	ax, word [the_mem]
	mov	es, ax
	push	ds
	push	es
	pop	ds
	mov	si, PAL_PTR
	; set color 0 to black
	xor	al, al
	mov	byte [si + 0], al
	mov	byte [si + 1], al
	mov	byte [si + 2], al
	; find color 0 in image and replace it
	mov	bx, IMAGE_PTR
	mov	cx, IMAGE_SIZE
fccc:
	mov	al, byte [bx]
	or	al, al
	jnz	fcccnext
	inc	al
	mov	byte [bx], al
fcccnext:
	inc	bx
	loop	fccc
	; now set the palette
	call	set_palette
	pop	ds


main_loop:

	; texx = (cos(a) * x - sin(a) * y) * scale

	; texy = (sin(a) * x + cos(a) * y) * scale
	
	; change rotation angle
	mov	bx, word [alpha]
	push	bx
	shl	bx, 1
	mov	si, word [_data + L_SINTAB + bx]
	mov	di, word [_data + L_COSTAB + bx]
	pop	ax
	inc	ax
	and	ax, MAX_DEGS - 1
	mov	word [alpha], ax

	
	; calculate lookups
	mov	bp, -160
	xor	bx, bx
	mov	cx, 320
prex:
	mov	ax, bp
	imul	si
	shl	edx, 16
	mov	dx, ax
	mov	dword [_data + L_SINX + bx], edx
	mov	ax, bp
	imul	di
	shl	edx, 16
	mov	dx, ax
	mov	dword [_data + L_COSX + bx], edx
	add	bx, 4
	inc	bp
	loop	prex

	mov	bp, (WINDOW_H / 2) * (-1)
	xor	bx, bx
	mov	cx, WINDOW_H
prey:
	mov	ax, bp
	imul	si
	shl	edx, 16
	mov	dx, ax
	mov	dword [_data + L_SINY + bx], edx
	mov	ax, bp
	imul	di
	shl	edx, 16
	mov	dx, ax
	mov	dword [_data + L_COSY + bx], edx
	add	bx, 4
	inc	bp
	loop	prey

	
	mov	ax, word [scr_buffer]
	mov	es, ax
	xor	di, di
	mov	ax, word [the_mem]
	mov	gs, ax

	; change scale
	mov	si, word [scale]
	mov	bx, word [scale_dir]
	add	si, bx
	mov	word [scale], si
	dec	bx
	jz	dir_is_1
	cmp	si, 256 / 16 ; 16 times bigger
	ja	dir_skip
dir_neg:
	neg	word [scale_dir]
	jmp	dir_skip
dir_is_1:
	cmp	si, 256 * 2 ; 2 times smaller
	ja	dir_neg	
dir_skip:
	

	; do zoomer
	xor	bp, bp
zoomy:
	xor	cx, cx
zoomx:
	mov	bx, cx
	mov	eax, dword [_data + L_COSX + bx]
	mov	bx, bp
	sub	eax, dword [_data + L_SINY + bx]
	shr	eax, 8
	mul	si
	shrd	ax, dx, 8
	add	ax, IMAGE_W / 2
	and	ax, IMAGE_W - 1
	push	ax

	mov	bx, cx
	mov	eax, dword [_data + L_SINX + bx]
	mov	bx, bp
	add	eax, dword [_data + L_COSY + bx]
	shr	eax, 8
	mul	si
	shrd	ax, dx, 8
	add	ax, IMAGE_H / 2
	and	ax, IMAGE_H - 1

	mov	bx, ax
	shl	bx, 8 ; IMAGE_W = 256 !!!
	pop	ax
	add	bx, ax
	mov	al, byte [gs:bx] ; IMAGE_PTR = 0
	stosb

	add	cx, 4
	cmp	cx, 320 * 4
	jne	zoomx

	add	bp, 4
	cmp	bp, WINDOW_H * 4
	jne	zoomy



	; copy scroll buffer
	mov	ax, word [scr_buffer]
	mov	es, ax
	mov	di, (WINDOW_H + 2) * 320
	mov	si, _data + SCROLL_PTR
	mov	cx, 320 * 8 / 4
	rep	movsd

	; move scroll
	push	ds
	pop	es
	mov	bx, _data + SCROLL_PTR
	mov	dx, 8
movescr_y:
	mov	di, bx
	mov	si, bx
	inc	si
	mov	cx, 319
	rep	movsb
	add	bx, 320
	dec	dx
	jnz	movescr_y

	; put one column of letter
	mov	ax, word [s_wait]
	or	ax, ax
	jz	sss_do
	dec	ax
	mov	word [s_wait], ax
	jmp	sss_done
sss_do:
	mov	di, _data + SCROLL_PTR + 319
	mov	si, s_text
	mov	ax, word [s_letter]
	add	si, ax
	lodsb
	xor	ah, ah
	sub	ax, 32
	shl	ax, 3
	add	ax, font_tab
	mov	si, ax
	mov	cl, byte [s_col]
	inc	cl
	mov	dh, 8
	mov	ah, byte [font_color]
sss_put:
	lodsb
	mov	dl, al
	mov	al, ah
	rcl	dl, cl
	jc	sss_p
	xor	al, al
sss_p:
	stosb
	add	di, 319
	dec	dh
	jnz	sss_put

	; change line and letter
	mov	al, byte [s_col]
	inc	al
	and	al, 7
	mov	byte [s_col], al
	jnz	sss_skip1
	
	mov	bx, word [s_letter]
	inc	bx
	mov	si, bx
	add	si, s_text
	lodsb
	or	al, al
	jnz	sss_skip2
	mov	word [s_wait], SCROLL_WAIT
	xor	bx, bx
sss_skip2:
	mov	word [s_letter], bx
sss_skip1:

sss_done:


	call	retrace

	; copy buffer to screen
	push	ds
	mov	ax, word [scr_buffer]
	mov	ds, ax
	xor	si, si
	mov	ax, 0x0a000
	mov	es, ax
	mov	di, 32000 - WINDOW_SIZE / 2 - (320 * (8 + 2) / 2)
	mov	cx, (WINDOW_SIZE + (320 * (8 + 2))) / 4
	rep	movsd
	pop	ds

	; check if ESC pressed
	in	al, 0x60
	dec	al
	jz	break_main_loop
	jmp	main_loop
break_main_loop:

	; set text mode
	mov	ax, 0x3
	int	0x10

free_:
	; free memory
	mov	ax, word [scr_buffer]
	call	free_seg
	mov	ax, word [the_mem]
	call	free_seg
quit_me:
	; quit to DOS
	mov	ah, 0x4c
	int	0x21

file_error:
	mov	ah, 0x9
	mov	dx, file_err
	int	0x21
	mov	dx, si
fff:
	lodsb
	or	al, al
	jnz	fff
	mov	di, si
	dec	di
	push	ds
	pop	es
	mov	al, 10
	stosb
	mov	al, 13
	stosb
	mov	al, '$'
	stosb
	mov	ah, 0x9
	int	21h
	jmp	free_

out_of_mem2:
	mov	ax, word [scr_buffer]
	call	free_seg

out_of_mem1:
	mov	ah, 0x9
	mov	dx, mem_err
	int	0x21
	jmp	quit_me

;----------------------------------------------------------------------

; *** input: ds:si = ptr to ASCIIZ string, cx = x, dx = y
;put_string:
;	mov	ax, word [scr_buffer]
;	mov	es, ax
;	mov	di, dx
;	shl	di, 6
;	shl	dx, 8
;	add	di, dx
;	add	di, cx
;
;	mov	dl, byte [font_color]
;
;ps_let:
;	lodsb
;	or	al, al
;	jnz	ps_ok
;	ret
;ps_ok:
;	sub	al, 32
;	xor	ah, ah
;	shl	ax, 3
;	mov	bx, ax
;
;	push	di
;	
;	mov	cx, 8
;ps_y:
;	push	cx
;	mov	cx, 8
;	mov	al, byte [font_tab + bx]
;	inc	bx
;ps_x:
;	rcl	al, 1
;	jnc	ps_next
;	mov	byte [es:di], dl
;ps_next:
;	inc	di
;	loop	ps_x
;	add	di, 320 - 8
;	pop	cx
;	loop	ps_y
;
;	pop	di
;	add	di, 8
;
;	jmp	ps_let
;	ret

;----------------------------------------------------------------------

%include 'procs.asm'
%include 'font.inc'

;----------------------------------------------------------------------

scr_buffer	dw 0
the_mem		dw 0

f_image		db 'Image.bin', 0
f_palette	db 'Palette.bin', 0
mem_err		db 'out of memory', 10, 13, '$'
file_err	db 'unable to load $'

angle_add	dd 0.0061359 ; PI / (MAX_DEGS / 2)
muler		dd 256.0

alpha		dw 0
scale		dw 0 ; fixed point 8.8
scale_dir	dw 1

font_color	db 0

s_letter	dw 0
s_col		db 0
s_wait		dw SCROLL_WAIT

s_text:
db 'CZESC!!! CZY ZBLIZAJACA SIE SESJA WZBUDZA W TOBIE LICZNE OBAWY? CZY SPODZIEWASZ SIE '
db 'OBLANYCH EGZAMINOW? A MOZE BOISZ SIE SKRESLENIA Z LISTY STUDENTOW? NIE CZEKAJ ZATEM '
db 'ANI CHWILI DLUZEJ! ODWIEDZ STRONE WWW.CHEMIA.W.PL!!! ZNAJDZIESZ TAM WSZYSTKO, CZEGO '
db 'POTRZEBUJE KAZDY SZANUJACY SIE STUDENT WYDZIALU CHEMICZNEGO: MATERIALY DYDAKTYCZNE, '
db 'PORADY, ROZWIAZANIA PROBLEMOW, LISTY ZADAN, OPRACOWANIA, TESTY, EGZAMINY I KOLOKWIA Z '
db 'POPRZEDNICH LAT... TO NIE KONIEC. ZAPRASZAMY NA NASZE FORUM, GDZIE MOZESZ '
db 'ZADAWAC NAM PYTANIA I ZAWSZE UZYSKASZ NIEZBEDNA POMOC! ZAPAMIETAJ NASZ ADRES: '
db 'WWW.CHEMIA.W.PL I ODWIEDZ NAS JUZ DZIS!!! ZALOGA CHEM-ON-LINE (MAJUMAE & KINGRATAE ;) )'
db '                                        '
db 'WIEM, ZE TEN TEKST JEST ZENUJACY, ALE LUDZIE! ODWIEDZAJCIE NASZA STRONE I WPISUJCIE SIE '
db 'NA FORUM BO BEZ TEGO STRONA UPADNIE :( PRZYNAJMNIEJ MAMY FAJNY DZIAL "HUMOR"... AHA, I '
db 'JESZCZE JEDNO: SORRY ZA PIKSELOZE, ALE TO JEST VGA POD DOSEM, WIEC SAMI ROZUMIECIE...', 0

_data:

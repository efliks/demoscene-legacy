
; returns requied memory in EAX
World2_GetMem:
	; World1 and World2 uses the same Floor1 structure
	xor	eax, eax
	ret

;************************************************************

; gets EAX and returns it unchanged :P
World2_Init:
	ret

;************************************************************

World2_Proc:
	mov	esi, o World2_World
	call	RenderWorld

	mov	esi, o Floor1_Strc ;World2_Strc
	mov	eax, FLOOR1_SIDE
	call	PlasmaFloor
	ret

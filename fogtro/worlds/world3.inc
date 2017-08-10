
World3_:

World3_World	dd o World3_Strc
		dd o World1_Camera
		dd 5
		dd o World3_Object_Table
		dd WF_DRAW_GOURAUD


World3_Object_Table:
Floor3_Obj	dd o Floor3_Strc
		dd 9 dup(0)
		dd 0, 0, 0
		dd 0, 0, 0
		dd 0, 0, 0
		dd OF_STATIC

Cube3_Obj	dd o Cube3_Strc
		dd 9 dup(0)
		dd 3.14, 0, 0
		dd 0, 0.034, 0
		dd 0, 0, 0
		dd OF_MOVEABLE

Cube31_Obj	dd o Cube31_Strc
		dd 9 dup(0)
		dd 0, 0, 0
		dd 0.119, 0.119, 0.119
		dd 0, 60.0, 0
		dd OF_MOVEABLE

Cube32_Obj	dd o Cube32_Strc
		dd 9 dup(0)
		dd 0, 0, 0
		dd 0.085, 0.085, 0.085
		dd 0, 60.0, 0
		dd OF_MOVEABLE

Cube33_Obj	dd o Cube33_Strc
		dd 9 dup(0)
		dd 0, 0, 0
		dd 0.051, 0.051, 0.051
		dd 0, 60.0, 0
		dd OF_MOVEABLE

World3_Struct_Table:
World3_Strc	dd FLOOR3_N_VERTS + 8 * 3 ; <-- add letter vertices !
		dd FLOOR3_N_FACES + 12 * 3 ; <-- add letter faces !
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

Floor3_Strc	dd FLOOR3_N_VERTS
		dd FLOOR3_N_FACES
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

; this is not a cube, but a letter :P
Cube3_Strc	dd 0
		dd 0
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

Cube31_Strc	dd 8
		dd 12
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

Cube32_Strc	dd 8
		dd 12
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

Cube33_Strc	dd 8
		dd 12
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

;************************************************************

WORLD3_SMEM	equ $ - World3_

W3_LETTER	equ 1

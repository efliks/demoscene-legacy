
World2_:

World2_World	dd o Floor1_Strc ;World2_Strc
		dd o World1_Camera ;World2_Camera
		dd 1
		dd o Floor1_Obj ;World2_Obj
		dd WF_DRAW_GOURAUD

;World2_Camera	dd 0, 0, 0
;		dd 0, 0, 0
;		dd 0
;		dd 9 dup(0)
;		dd 0, 0, 0
;		dd CF_NO_TARGET


;World2_Obj	dd o World2_Strc
;		dd 9 dup(0)
;		dd 0, 0, 0
;		dd 0, 0, 0
;		dd 0, 0, 0
;		dd OF_STATIC

;World2_Strc	dd FLOOR2_N_VERTS
;		dd FLOOR2_N_FACES
;		dd 0
;		dd 0, 0, 0, 0, 0, 0, 0, 0

;************************************************************

WORLD2_SMEM	equ $ - World2_

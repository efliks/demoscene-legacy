
World1_:

World1_World	dd o World1_Strc
		dd o World1_Camera
		dd (1 + W1_TEXT3D_LEN)
		dd o World1_ObjTab
		dd WF_DRAW_GOURAUD

World1_Camera	dd 0, 0, 0
		dd 0, 0, 0
		dd 0
		dd 9 dup(0)
		dd 0, 0, 0
		dd CF_NO_TARGET


World1_ObjTab:

Floor1_Obj	dd o Floor1_Strc
		dd 9 dup(0)
		dd 0, 0, 0
		dd 0, 0, 0
		dd 0, 0, 0
		dd OF_STATIC

LetTab1_Obj	db (W1_TEXT3D_LEN * OBJECT3D_SIZE) dup(0)

;Letter1_Obj	dd o Letter1_Strc
;		dd 9 dup(0)
;		dd 0, 0, 0
;		dd 0, 0.017, 0
;		dd 0, 0, 0
;		dd OF_MOVEABLE

; 3d structures:

World1_Strc	dd 0
		dd 0
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

Floor1_Strc	dd FLOOR1_N_VERTS
		dd FLOOR1_N_FACES
		dd 0
		dd 0, 0, 0, 0, 0, 0, 0, 0

LetTab1_Strc	db (W1_TEXT3D_LEN * STRUCT3D_SIZE) dup(0)

;Letter1_Strc	dd 0
;		dd 0
;		dd 0
;		dd 0, 0, 0, 0, 0, 0, 0, 0	

;************************************************************

WORLD1_SMEM	equ $ - World1_

W1_TrackV1	dd -700.0, 250.0, -800.0
W1_TrackV2	dd 900.0, 800.0, 500.0
W1_TrackV3	dd 900.0, 300.0, -700.0
W1_TrackV4	dd -500.0, 700.0, 800.0
WORLD1_T_VERTS	equ ($ - W1_TrackV1) / 12

World1_Track1	dd 0, 0, 0
World1_Track2	dd 0, 0, 0
World1_Track3	dd 0, 0, 0
World1_Track4	dd 0, 0, 0
WORLD1_TRACKS	equ (($ - World1_Track1) / TRACK_SIZE)

W1_text3d	db '$'
W1_TEXT3D_LEN	equ ($ - W1_text3d)

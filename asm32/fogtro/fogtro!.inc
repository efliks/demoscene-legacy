
b		equ byte ptr
w		equ word ptr
d		equ dword ptr
o		equ offset

;************************************************************
;	constants
;************************************************************

MAX_STR_LEN	equ 256
T_SPEED		equ 6
T_CURSOR	equ 254
T_WAIT_TIME	equ 110

REQ_MEM		equ 64000 + SMALL_SCR_MEM + 768 + MAX_STR_LEN
TEXT_COLOR	equ 63 - 16 ;32 + 64 + 64
PART_TIMEOUT	equ 2000	; timeout in frames

MAX_COLORS  equ     255

global  copy_screen_buffer:PROC
global  clear_screen_buffer:PROC
global  alloc_screen_buffer:PROC
global  free_screen_buffer:PROC
global  wait_for_frame:PROC
global  buffer_seg:WORD


_TEXT   segment byte    public  'CODE'
_TEXT   ends
_DATA   segment word    public  'DATA'
_DATA   ends
_STACK  segment PARA    stack   'STACK'
_STACK  ends

DGROUP  GROUP _DATA, _STACK
ASSUME  cs:_TEXT, ds:DGROUP, ss:DGROUP


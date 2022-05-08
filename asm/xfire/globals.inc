_TEXT   segment byte    public  'CODE'
_TEXT   ends
_DATA   segment word    public  'DATA'
_DATA   ends
_STACK  segment PARA    stack   'STACK'
_STACK  ends

DGROUP  GROUP _DATA, _STACK
ASSUME  cs:_TEXT, ds:DGROUP, ss:DGROUP


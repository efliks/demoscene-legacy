@echo off

c:\asm16\tasm d:\asmshade\asmshade
c:\asm16\tlink d:\asmshade\asmshade /t /3

del *.map
del *.obj

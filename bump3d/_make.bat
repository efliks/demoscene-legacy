@echo off

rem c:\asm16\tasm d:\bump3d\kernel\kernel /m

c:\asm16\tasm d:\bump3d\bump3d /m
c:\asm16\tlink d:\bump3d\bump3d kernel\kernel,bump3d20.exe /3 /x

del *.obj

upx bump3d20.exe

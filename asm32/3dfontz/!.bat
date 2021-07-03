@echo off

c:\asm16\tasm d:\3dfontz\3dfontz /m
c:\asm16\tlink d:\3dfontz\3dfontz kernel\kernel,3dfontz.exe /3 /x

del 3dfontz.obj

rem d:\upx d:\3dfontz\3dfontz.exe

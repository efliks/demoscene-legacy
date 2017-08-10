@echo off

rem c:\asm16\tasm d:\fogtro!\kernel\kernel /m /p /q
rem move d:\fogtro!\kernel.obj d:\fogtro!\kernel\

c:\asm16\tasm d:\fogtro!\fogtro! /m /p /q
c:\asm16\tlink d:\fogtro!\fogtro! kernel\kernel,fogtro!.exe /3 /x

del *.obj

rem upx fogtro!.exe

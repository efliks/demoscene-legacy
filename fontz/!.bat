@echo off

bcc32 fontz.c fontz\letgen.obj

del *.obj
del *.tds

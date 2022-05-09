xfire.exe: xfire.obj graphlib.obj
	tlink xfire.obj graphlib.obj

xfire.obj: xfire.asm globals.inc
	tasm xfire.asm

graphlib.obj: graphlib.asm globals.inc
	tasm graphlib.asm


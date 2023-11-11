# To build, type in:
# c:\matrixc > c:\tc\bin\make.exe -f makefile

CC=c:\tc\bin\tcc.exe
LINK=c:\tc\bin\tlink.exe

CFLAGS=-G -a -ml -c

all: matriks.exe

matriks.exe: matrix.obj gfx.obj fontnew.obj clock.obj
	$(LINK) c0l matrix.obj gfx.obj fontnew.obj clock.obj,matriks.exe,,cl -Lc:\tc\lib

matrix.obj: matrix.c
	$(CC) $(CFLAGS) matrix.c

gfx.obj: gfx.c
	$(CC) $(CFLAGS) gfx.c

fontnew.obj: fontnew.c
	$(CC) $(CFLAGS) fontnew.c

clock.obj: clock.c
	$(CC) $(CFLAGS) clock.c

clean:
	del *.obj *.map matriks.exe

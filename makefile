TASM = c:\tasm\bin\tasm.exe
TLINK = c:\tasm\bin\tlink.exe

all: blurline.com bumpmap.com cells.com explode.com lamp.com matrix.com movback.com plasma.com rotxor.com rtplasma.com sinexpl.com sinxor.com xfire.com xstars.com asmshade.com wireblur.com flag3d.com morph3d.com fixed3d.com

blurline.com: blurline.obj sys.obj
	$(TLINK) /t blurline.obj sys.obj,blurline.com

bumpmap.com: bumpmap.obj sys.obj
	$(TLINK) /t bumpmap.obj sys.obj,bumpmap.com

cells.com: cells.obj sys.obj
	$(TLINK) /t cells.obj sys.obj,cells.com

explode.com: explode.obj sys.obj
	$(TLINK) /t explode.obj sys.obj,explode.com

lamp.com: lamp.obj sys.obj
	$(TLINK) /t lamp.obj sys.obj,lamp.com

matrix.com: matrix.obj sys.obj
	$(TLINK) /t matrix.obj sys.obj,matrix.com

movback.com: movback.obj sys.obj
	$(TLINK) /t movback.obj sys.obj,movback.com

plasma.com: plasma.obj sys.obj
	$(TLINK) /t plasma.obj sys.obj,plasma.com

rotxor.com: rotxor.obj sys.obj
	$(TLINK) /t rotxor.obj sys.obj,rotxor.com

rtplasma.com: rtplasma.obj sys.obj
	$(TLINK) /t rtplasma.obj sys.obj,rtplasma.com

sinexpl.com: sinexpl.obj sys.obj
	$(TLINK) /t sinexpl.obj sys.obj,sinexpl.com

sinxor.com: sinxor.obj sys.obj
	$(TLINK) /t sinxor.obj sys.obj,sinxor.com

xfire.com: xfire.obj sys.obj
	$(TLINK) /t xfire.obj sys.obj,xfire.com

xstars.com: xstars.obj sys.obj
	$(TLINK) /t xstars.obj sys.obj,xstars.com

asmshade.com: asmshade.obj data.obj flat3.obj grd3.obj tex3.obj math3d.obj sys.obj
	$(TLINK) /t asmshade.obj data.obj flat3.obj grd3.obj tex3.obj math3d.obj sys.obj,asmshade.com

wireblur.com: wireblur.obj math3d.obj sys.obj
	$(TLINK) /t wireblur.obj math3d.obj sys.obj,wireblur.com

flag3d.com: flag3d.obj math3d.obj flat3.obj sys.obj
	$(TLINK) /t flag3d.obj math3d.obj flat3.obj sys.obj,flag3d.com

morph3d.com: morph3d.obj math3d.obj sys.obj
	$(TLINK) /t morph3d.obj math3d.obj sys.obj,morph3d.com

fixed3d.com: fixed3d.obj flat3.obj sys.obj
	$(TLINK) /t fixed3d.obj flat3.obj sys.obj,fixed3d.com

.asm.obj:
	$(TASM) $<

clean:
	del *.obj *.map *.com

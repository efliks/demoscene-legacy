CXX=gcc
INCLUDE=-I.
FLAGS=-W -Wall -pedantic


all: flatgrd.exe

flatgrd.exe: 3dmath.o 3dobj.o bumpmap.o bump_tri.o dump_scr.o envmap.o flat_tri.o font.o graph.o grd_tri.o main.o phongpal.o tex_tri.o 
	$(CXX) $^ -o $@ -lm

main.o: main.c
	$(CXX) $(FLAGS) $(INCLUDE) -o main.o -c main.c

graph.o: graph.c
	$(CXX) $(FLAGS) $(INCLUDE) -o graph.o -c graph.c

font.o: font.c
	$(CXX) $(FLAGS) $(INCLUDE) -o font.o -c font.c

3dmath.o: 3dmath.c
	$(CXX) $(FLAGS) $(INCLUDE) -o 3dmath.o -c 3dmath.c

3dobj.o: 3dobj.c
	$(CXX) $(FLAGS) $(INCLUDE) -o 3dobj.o -c 3dobj.c

flat_tri.o: flat_tri.c
	$(CXX) $(FLAGS) $(INCLUDE) -o flat_tri.o -c flat_tri.c

grd_tri.o: grd_tri.c
	$(CXX) $(FLAGS) $(INCLUDE) -o grd_tri.o -c grd_tri.c

tex_tri.o: tex_tri.c
	$(CXX) $(FLAGS) $(INCLUDE) -o tex_tri.o -c tex_tri.c

bump_tri.o: bump_tri.c
	$(CXX) $(FLAGS) $(INCLUDE) -o bump_tri.o -c bump_tri.c

envmap.o: envmap.c
	$(CXX) $(FLAGS) $(INCLUDE) -o envmap.o -c envmap.c

bumpmap.o: bumpmap.c
	$(CXX) $(FLAGS) $(INCLUDE) -o bumpmap.o -c bumpmap.c

phongpal.o: phongpal.c
	$(CXX) $(FLAGS) $(INCLUDE) -o phongpal.o -c phongpal.c

dump_scr.o: dump_scr.c
	$(CXX) $(FLAGS) $(INCLUDE) -o dump_scr.o -c dump_scr.c

.PHONY: clean
clean:
	-rm -f flatgrd.exe *.o &> /dev/null


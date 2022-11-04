CXX=gcc
INCLUDE=-I.
FLAGS=-W -Wall -pedantic


all: flatgrd.exe

flatgrd.exe: 3dmath.o 3dobj.o bumpmap.o dump_scr.o envmap.o font.o graph.o main.o phongpal.o triangle.o
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

triangle.o: triangle.c
	$(CXX) $(FLAGS) $(INCLUDE) -o triangle.o -c triangle.c

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


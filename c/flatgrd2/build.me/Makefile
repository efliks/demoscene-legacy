# Before running make: source /home/mikolaj/local/opt/djgpp-12.1.0/setenv  

CC=gcc
INCLUDE=-I.
FLAGS=-W -Wall -pedantic -O2

SRC=../source


all: flatgrd2.exe

flatgrd2.exe: 3dmath.o 3dobj.o bumpmap.o dump_scr.o envmap.o font.o graph.o main.o phongpal.o triangle.o
	$(CC) $^ -o $@ -lm

main.o: $(SRC)/main.c
	$(CC) $(FLAGS) $(INCLUDE) -o main.o -c $(SRC)/main.c

graph.o: $(SRC)/graph.c
	$(CC) $(FLAGS) $(INCLUDE) -o graph.o -c $(SRC)/graph.c

font.o: $(SRC)/font.c
	$(CC) $(FLAGS) $(INCLUDE) -o font.o -c $(SRC)/font.c

3dmath.o: $(SRC)/3dmath.c
	$(CC) $(FLAGS) $(INCLUDE) -o 3dmath.o -c $(SRC)/3dmath.c

3dobj.o: $(SRC)/3dobj.c
	$(CC) $(FLAGS) $(INCLUDE) -o 3dobj.o -c $(SRC)/3dobj.c

triangle.o: $(SRC)/triangle.c
	$(CC) $(FLAGS) $(INCLUDE) -o triangle.o -c $(SRC)/triangle.c

envmap.o: $(SRC)/envmap.c
	$(CC) $(FLAGS) $(INCLUDE) -o envmap.o -c $(SRC)/envmap.c

bumpmap.o: $(SRC)/bumpmap.c
	$(CC) $(FLAGS) $(INCLUDE) -o bumpmap.o -c $(SRC)/bumpmap.c

phongpal.o: $(SRC)/phongpal.c
	$(CC) $(FLAGS) $(INCLUDE) -o phongpal.o -c $(SRC)/phongpal.c

dump_scr.o: $(SRC)/dump_scr.c
	$(CC) $(FLAGS) $(INCLUDE) -o dump_scr.o -c $(SRC)/dump_scr.c

.PHONY: clean
clean:
	-rm -f flatgrd2.exe *.o &> /dev/null


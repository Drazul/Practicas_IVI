DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ansi -std=c++11
LDFLAGS := -lGL -lGLU -lglut -lGLEW
CC := g++

all: dirs loadMesh runLoadMesh

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

triangles: $(DIROBJ)triangles.o $(DIROBJ)LoadShaders.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

loadMesh: $(DIROBJ)loadMesh.o $(DIROBJ)LoadShaders.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

readFile: $(DIROBJ)readFile.o $(DIROBJ)LoadShaders.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)


run:
	./exec/triangles

runLoadMesh:
	./exec/loadMesh

runReadFile:
	./exec/readFile

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ansi
LDFLAGS := -lglut -lGLU -lGL
CC := gcc

#all: dirs helloOpenGL tetera planetario

all: dirs ejercicio2

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

ejercicio1: $(DIROBJ)ejercicio1.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)
	./exec/ejercicio1

ejercicio2: $(DIROBJ)ejercicio2.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)
	./exec/ejercicio2

helloOpenGL: $(DIROBJ)helloOpenGL.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)
	./exec/helloOpenGL

tetera: $(DIROBJ)tetera.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)
	./exec/tetera

planetario: $(DIROBJ)planetario.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)
	./exec/planetario

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

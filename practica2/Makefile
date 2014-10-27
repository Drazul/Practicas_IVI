DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ansi
LDFLAGS := -lglut -lGLU -lGL
CC := gcc

all: dirs helloOpenGL tetera planetario

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

helloOpenGL: $(DIROBJ)helloOpenGL.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

tetera: $(DIROBJ)tetera.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

planetario: $(DIROBJ)planetario.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

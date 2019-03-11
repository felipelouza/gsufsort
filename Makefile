CC = gcc
CFLAGS += -Wall
#CFLAGS += -g -O0
CFLAGS += -D_FILE_OFFSET_BITS=64 -m64 -O3 -fomit-frame-pointer -Wno-char-subscripts 

LFLAGS = -lm -ldl

##

LIBOBJ = \
	lib/file.o\
	lib/utils.o\
	external/gsacak.o\
	external/malloc_count/malloc_count.o
	
LIBOBJ_64 = \
	lib/file.64.o\
	lib/utils.64.o\
	external/gsacak.64.o\
	external/malloc_count/malloc_count.64.o

##

DEBUG = 0
M64 = 0

##

DEFINES = -DDEBUG=$(DEBUG) 

CFLAGS += $(DEFINES)

all: compile


##

DIR = dataset/
INPUT = input-10000.txt
DOCS	= 3

##

all: compile compile-64

clean:
	\rm -f *.o  external/*.o lib/*o external/malloc_count/*.o gsufsort gsufsort-64 

remove:
	\rm -f $(DIR)*.bin  $(DIR)*.sa $(DIR)*.da $(DIR)*.lcp $(DIR)*.bwt $(DIR)*.gsa $(DIR)*.gesa

##

%.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=0

%.64.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=1	

##

compile: main.c ${LIBOBJ} 
	$(CC) -o gsufsort main.c ${LIBOBJ} $(CFLAGS) $(LFLAGS) -DM64=0

compile-64: main.c ${LIBOBJ_64} 
	$(CC) -o gsufsort-64 main.c ${LIBOBJ_64} $(CFLAGS) $(LFLAGS)  -DM64=1 

run:
	./gsufsort $(DIR)$(INPUT) -v --sa

valgrind:
	valgrind -q --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./gsufsort $(DIR)$(INPUT) --sa -d 100

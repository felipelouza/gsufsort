CC = gcc
CFLAGS += -Wall
CFLAGS += -g -O0
#CFLAGS += -D_FILE_OFFSET_BITS=64 -m64 -O3 -fomit-frame-pointer -Wno-char-subscripts 

LFLAGS = -lm -ldl

##

LIBOBJ = \
	lib/file.o\
	lib/utils.o\
	external/gsacak.o\
	external/malloc_count/malloc_count.o
	
##

DEBUG = 0
M64 = 0

##

DEFINES = -DDEBUG=$(DEBUG) -DM64=$(M64) 

CFLAGS += $(DEFINES)

all: compile


##

DIR = dataset/
INPUT = input-10000.txt
DOCS	= 3

##

all: compile

clean:
	\rm -f *.o  external/*.o lib/*o external/malloc_count/*.o gsufsort

remove:
	\rm -f $(DIR)*.bin  $(DIR)*.sa $(DIR)*.da $(DIR)*.lcp $(DIR)*.bwt $(DIR)*.gsa $(DIR)*.gesa

##

compile: main.c ${LIBOBJ} 
	$(CC) -o gsufsort main.c ${LIBOBJ} $(CFLAGS) $(LFLAGS) 

run:
	./gsufsort $(DIR)$(INPUT) -v --sa

valgrind:
	valgrind -q --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./gsufsort $(DIR)$(INPUT) --sa -d 100

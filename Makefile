CC = gcc
CXX = /usr/bin/c++ 
LIB_DIR = ${HOME}/lib
INC_DIR = ${HOME}/include

#WFLAGS= -Wall -Wextra  -DNDEBUG -Wno-ignored-qualifiers
WFLAGS= -Wall -Wno-unused-function 
OPT_FLAGS= -O3 -ffast-math -funroll-loops -m64 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64
#CCLIB= -lstdc++ -lsdsl  -I$(INC_DIR) -L$(LIB_DIR)
LFLAGS = -lm -ldl -mpopcnt

CFLAGS = $(WFLAGS) $(OPT_FLAGS) 
CFLAGS += $(LFLAGS)

CXX_FLAGS = -std=c++11 $(WFLAGS) $(OPT_FLAGS)
CXX_FLAGS += -I$(INC_DIR) -L$(LIB_DIR) $(LFLAGS)

##

LIBOBJ = \
	lib/file.o\
	lib/utils.o\
	lib/rankbv.o\
	external/gsacak.o\
	external/malloc_count/malloc_count.o
	
LIBOBJ_64 = \
	lib/file.64.o\
	lib/utils.64.o\
	lib/rankbv.64.o\
	external/gsacak.64.o\
	external/malloc_count/malloc_count.64.o

##

DEBUG = 0
M64 = 0

##

DEFINES = -DDEBUG=$(DEBUG) 
GZ = 0

CFLAGS += $(DEFINES) -DGZ=$(GZ)

ifeq ($(GZ),1)
	CFLAGS += -lz
endif
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

###

%.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=0

%.64.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=1	

##

compile: main.c ${LIBOBJ} 
	$(CC) -o gsufsort main.c ${LIBOBJ} $(CFLAGS) $(CCLIB) -DM64=0

compile-64: main.c ${LIBOBJ_64} 
	$(CC) -o gsufsort-64 main.c ${LIBOBJ_64} $(CFLAGS) $(CCLIB) -DM64=1 

run:
	./gsufsort $(DIR)$(INPUT) -v --sa

valgrind:
	valgrind -q --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./gsufsort $(DIR)$(INPUT) --sa -d 100

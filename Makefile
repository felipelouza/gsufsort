CC = gcc
CXX = /usr/bin/c++ 
LIB_DIR = ${HOME}/lib
INC_DIR = ${HOME}/include

#WFLAGS= -Wall -Wextra  -DNDEBUG -Wno-ignored-qualifiers
WFLAGS= -Wall 
OPT_FLAGS= -O3 -ffast-math -funroll-loops -m64 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64
CCLIB= -lstdc++ -lsdsl  -I$(INC_DIR) -L$(LIB_DIR)
LFLAGS = -lm -ldl 
#-mpopcnt

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
LIGHT = 1
SDSL = 1

##

DEFINES = -DDEBUG=$(DEBUG) -DLIGHT=$(LIGHT) -DSDSL=$(SDSL)

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

###

document_array: 
	$(CXX) $(CXX_FLAGS) -c lib/document_array.cpp -o lib/document_array.o -DM64=0

document_array.64: 
	$(CXX) $(CXX_FLAGS) -c lib/document_array.cpp -o lib/document_array.64.o -DM64=0

###

%.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=0

%.64.o: %.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=1	

##

compile: main.c ${LIBOBJ} document_array 
	$(CC) -o gsufsort main.c ${LIBOBJ} lib/document_array.o $(CFLAGS) $(CCLIB) -DM64=0

compile-64: main.c ${LIBOBJ_64} document_array.64 
	$(CC) -o gsufsort-64 main.c ${LIBOBJ_64} lib/document_array.64.o $(CFLAGS) $(CCLIB) -DM64=1 

run:
	./gsufsort $(DIR)$(INPUT) -v --sa

valgrind:
	valgrind -q --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./gsufsort $(DIR)$(INPUT) --sa -d 100

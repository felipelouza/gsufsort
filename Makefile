CC?=gcc

#WFLAGS= -Wall -Wextra  -DNDEBUG -Wno-ignored-qualifiers
WFLAGS= -Wall -Wno-unused-function #-Werror 
OPT_FLAGS= -O3 -ffast-math -funroll-loops -m64 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64
CFLAGS = $(WFLAGS) $(OPT_FLAGS)
SYSTEM?=$(shell uname -s)

#valgrind
#CFLAGS += -g -O0 
##

MALLOC_COUNT=
MALLOC_COUNT64=
 
ifeq ($(SYSTEM),Darwin)
  LDFLAGS= -mpopcnt
else
	MALLOC_COUNT=external/malloc_count/malloc_count.o
	MALLOC_COUNT64=external/malloc_count/malloc_count.64.o
  LDFLAGS= -lm -ldl -mpopcnt
endif

LIBOBJ = \
	lib/file.o\
	lib/utils.o\
	lib/rankbv.o\
	external/gsacak.o ${MALLOC_COUNT}
	
LIBOBJ_64 = \
	lib/file.64.o\
	lib/utils.64.o\
	lib/rankbv.64.o\
	external/gsacak.64.o ${MALLOC_COUNT}

TARGETS=gsufsort gsufsort-64

##

DEBUG = 0
M64 = 0
DNA = 0
TERMINATOR = 1 
DEFINES = -DDEBUG=$(DEBUG)
ifeq ($(DNA),1)
	DEFINES += -DDNA=$(DNA)
endif
DEFINES += -DTERMINATOR=$(TERMINATOR)
GZ = 1
CFLAGS += $(DEFINES) -DGZ=$(GZ)
ifeq ($(GZ),1)
  LDFLAGS += -lz
endif
##

DIR = dataset/
INPUT = input.txt
DOCS	= 3

##

all:${TARGETS}

clean:
	@${RM} *.o external/*.o lib/*o external/malloc_count/*.o ${TARGETS}

remove:
	@for directory in . dataset; do\
	  echo $$directory; \
	    for suffix in lcp str bwt gsa sa da gesa str qs ibwt iqs; do \
      	${RM} $$directory/*.$$suffix;\
	  done \
	done

###

%.o:%.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=0

%.64.o:%.c
	 $(CC) $(CFLAGS) -c -o $@ $< -DM64=1	

##

gsufsort:main.c ${LIBOBJ}
	$(CC) -o $@ main.c ${LIBOBJ} $(CFLAGS) $(LDFLAGS) $(CCLIB) -DM64=0

gsufsort-64:main.c ${LIBOBJ_64}
	$(CC) -o $@ main.c ${LIBOBJ_64} $(CFLAGS) $(LDFLAGS) $(CCLIB) -DM64=1

run:${TARGETS}
	./gsufsort $(DIR)$(INPUT) -v --sa
	./gsufsort-64 $(DIR)$(INPUT) -v --sa

run_all:${TARGETS}
	@for directory in dataset; do\
	  for filename in `ls $$directory/*.txt $$directory/*.fasta $$directory/*.fastq`; do\
	    for prog in ${TARGETS}; do\
                ./$$prog $$filename --sa --lcp --da --gsa --gesa --bwt --str;\
	    done \
	  done \
        done

valgrind:
	valgrind -q --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./gsufsort $(DIR)$(INPUT) --sa -d 100

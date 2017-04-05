
CC = gcc

CFLAGS = \
	-O2\
	-I.\
	-I../include
#	-fomit-frame-pointer

OBJDIR = ../obj
LIBDIR = ../lib
INCDIR = ../include
SNAPDIR = ../SNAP
LIB = $(LIBDIR)/libmovie.a

SRC = error.c\
	image.c\
	input.c\
	movieobj.c\
	response.c\
	video.c\
	tsc_time.c\
	parport.c\
	exptlib1.c

OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))

VPATH = . 

all : $(OBJ) $(LIB)

include makefile.dep

$(OBJDIR)/graph.o : $(SNAPDIR)/graph.o
	-cp $(SNAPDIR)/graph.o $(OBJDIR)/graph.o

$(OBJDIR)/%.o: %.c
	-$(CC) $(CFLAGS) -o $@ -c $<

$(LIB): $(OBJ) $(OBJDIR)/graph.o
	-rm $(LIB)
	-ar src $(LIB) $(OBJ) $(OBJDIR)/graph.o

install:
	

clean:
	-rm $(OBJ)
	-rm $(LIB)

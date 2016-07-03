INCDIR = 
LIBDIR = 
DEBUG =
PROFILE =
OPTIM = -ggdb3 #-march=pentium-m -O2
WARNING = -Wall -Wextra

#################################################
CC = gcc
CXX = g++

CPPFLAGS = -I.  `sdl-config --cflags`

CFLAGS   =  $(WARNING) $(OPTIM) $(DEBUG) $(PROFILE)
CXXFLAGS =  $(WARNING) $(OPTIM) $(DEBUG) $(PROFILE) 

LDFLAGS = -L. -L$(LIBDIR)
LOADLIBES = 

CFILES=$(wildcard *.c)
OBJFILES=$(patsubst %.c, %.o, $(CFILES))

all: lib3d.a 
	ar rcs libguo3d.a *.o
	
lib3d.a: $(OBJFILES)


clean:
	rm -rf *.h~ *.c~ *.o Makefile~ *.*.orig *.a *~


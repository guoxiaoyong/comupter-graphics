INCDIR = 
LIBDIR = -L../src/
DEBUG =
PROFILE =
OPTIM = -ggdb3
WARNING = -W -Wall -Wextra

#################################################
CC = gcc
CXX = g++

CPPFLAGS = -I.  -I.. -I../src/

CFLAGS   =  $(WARNING) $(OPTIM) $(DEBUG) $(PROFILE) #`sdl-config --cflags`
CXXFLAGS =  $(WARNING) $(OPTIM) $(DEBUG) $(PROFILE) 

LDFLAGS = $(LIBDIR)
LDLIBS= #`sdl-config --static-libs`
LOADLIBES = -lguo3d 

all: obj_test tcimg_test1 tcimg_test2 geo_test

clean:
	rm -rf *.o *.exe *.*~ Makefile~ *.*.orig *.stackdump


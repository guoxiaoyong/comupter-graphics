INCDIR = 
LIBDIR = -L../src/
DEBUG =
PROFILE =
OPTIM = -ggdb3
WARNING = -W -Wall -Wextra

#################################################
CC = gcc
CXX = g++

CPPFLAGS = -I.  -I.. -I../src/ -I../data/ 

CFLAGS   =  $(WARNING) $(OPTIM) $(DEBUG) $(PROFILE) #`sdl-config --cflags`
CXXFLAGS =  $(WARNING) $(OPTIM) $(DEBUG) $(PROFILE) 

LDFLAGS = $(LIBDIR)
LDLIBS= `sdl-config --static-libs`
LOADLIBES = -lguo3d 

all: sphere_backface simpleface_orig simpleface_reco povexport raytracer sphere zbuffer jpeg
#all: jpeg

clean:
	rm -rf *.o *.exe *.*~ Makefile~ *.*.orig *.stackdump


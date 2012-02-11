CFLAGS=-Wall -pedantic -g -I/usr/X11R6/include `pkg-config --cflags cairo`
LDFLAGS=-Wall -g `pkg-config --libs cairo` -L/usr/X11R6/lib -lX11

all: robots

robots: robots.o
	g++ -o robots ${LDFLAGS} robots.o

robots.o: robots.cpp
	g++ -c robots.cpp ${CFLAGS} -o robots.o

CFLAGS=-Wall -pedantic -g -I/usr/X11R6/include `pkg-config --cflags cairo`
LDFLAGS=-Wall -g `pkg-config --libs cairo` -L/usr/X11R6/lib -lX11

all: robots

robots: robots.o astar.o visualizer.o
	g++ -o robots ${LDFLAGS} robots.o astar.o visualizer.o

robots.o: robots.cpp robots.h astar.h visualizer.h
	g++ -c robots.cpp ${CFLAGS} -o robots.o

astar.o: astar.cpp astar.h
	g++ -c astar.cpp ${CFLAGS} -o astar.o

visualizer.o: visualizer.cpp visualizer.h
	g++ -c visualizer.cpp ${CFLAGS} -o visualizer.o

CC = g++
DEBUG = -g -O0 -pedantic-errors
CFLAGS = -Wall -std=c++11 -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

project1: project1.o
	$(CC) $(LFLAGS) -o project1 project1.o -lmenu -lncurses

project1.o: project1.cpp
	$(CC) $(CFLAGS) project1.cpp

.PHONY: clean

clean: 
	rm -rf *.o
	rm -rf main

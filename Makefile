# Makefile for Project Maze



PROG = AMStartup 
OBJS = AMStartup.o mazeSolver.o avatar.o graphics.o 

#PROG1 = designTest
#OBJS1 = avatar.o mazeSolver.o graphics.o designTest.o

PROG2 = graphicstest
OBJS2 = graphics.o mazeSolver.o avatar.o graphicstest.o

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -lpthread 
CC = gcc
MAKE = make

all: $(PROG) $(PROG2) #$(PROG1)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lcurses --disable-leaks

#$(PROG1): $(OBJS1)
#	$(CC) $(CFLAGS) $^ -o $@ -lcurses --disable-leaks

$(PROG2): $(OBJS2)
	$(CC) $(CFLAGS) $^ -o $@ -lcurses --disable-leaks


AMStartup.o: amazing.h mazeSolver.h avatar.h
mazeSolver.o: amazing.h avatar.h
graphics.o: mazeSolver.h graphics.h 
avatar.o: graphics.h amazing.h  	
graphicstest.o: avatar.h mazeSolver.h graphics.h
#designTest.o: avatar.h mazeSolver.h


.PHONY: clean test

clean: 
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f $(PROG2)
	rm -f stocks
	rm -f *core*
	rm -f log.out -r
	mkdir log.out

test:
	 bash testing.sh

CC = gcc
CFLAGS = -g -std=c99 -Wall

# -std=gnu99

all: minesweeper

minesweeper: bin/minesweeper.o bin/board.o bin/tile.o
	$(CC) -o minesweeper bin/* #-lncurses

bin/minesweeper.o: src/minesweeper.c src/board.h src/tile.h
	$(CC) $(CFLAGS) -c -o bin/minesweeper.o src/minesweeper.c

bin/board.o: src/board.c src/board.h src/tile.h
	$(CC) $(CFLAGS) -c -o bin/board.o src/board.c

bin/tile.o: src/tile.c src/tile.h
	$(CC) $(CFLAGS) -c -o bin/tile.o src/tile.c

.PHONY: clean

clean:
	rm -rf bin/*

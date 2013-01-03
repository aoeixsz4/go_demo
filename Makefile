CFLAGS=-I/usr/include/SDL -ggdb
LDFLAGS=-lSDL -lSDL_draw
CC=gcc
OBJS=demo.o display.o board.o event.o

all: demo

clean:
	$(RM) $(OBJS)

demo: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

CC=cc
CFLAGS=-O2 -Wall -Wextra -pedantic -std=c11
LDLIBS=-lncursesw -lm
TARGET=flip-bits
OBJS=main.o board.o draw.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(TARGET) $(LDLIBS)


clean:
	rm -f *.o
	rm -f $(TARGET)


# for f in *.c; do gcc -MM -MT ${f%.c}.o $f; done
board.o: board.c board.h common.h
draw.o: draw.c draw.h board.h common.h
main.o: main.c board.h common.h draw.h

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<


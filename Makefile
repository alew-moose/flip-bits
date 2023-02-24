CFLAGS=-O2 -Wall -Wextra -pedantic -std=c11
LDLIBS=-lncursesw -lm
TARGET=flip-bits
OBJS=main.o board.o draw.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(TARGET) $(LDLIBS)


clean:
	@-rm -f *.o
	@-rm -f $(TARGET)

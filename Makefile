CFLAGS=-O2 -Wall -Wextra -pedantic -std=c11
LDLIBS=-lncursesw -lm
TARGET=flip-bits

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(TARGET) $(LDLIBS)


clean:
	@-rm -f $(TARGET)

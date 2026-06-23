CC = cc
CFLAGS = -std=c99 -Wall -Wextra -O2
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = maze

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c maze.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

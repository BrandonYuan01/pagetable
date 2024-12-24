CC = gcc
CFLAGS = -Wall
LDFLAGS = -Wall
LDLIBS = -lm

.PHONY: all clean

all: libmlpt.a

libmlpt.a: pages.o
	ar -rcs libmlpt.a pages.o

%.o: %.c %.h config.h mlpt.h
	$(CC) -c $(CFLAGS) $< -o $@

pages: libmlpt.a 
	$(CC) $(LDFLAGS) -o pages libmlpt.a

clean:
	rm --force pages.o libmlpt.a pages 
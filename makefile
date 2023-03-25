CC       = gcc
CPPFLAGS =
CFLAGS   = -Wall -Wextra -std=c11 -O2
LDFLAGS  =
INCLUDE  = structs.h

.PHONY: all clean

all: similar_lines

similar_lines: main.o find-groups-of-similar-lines.o print-groups-and-free-memory.o parse-text.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.o find-groups-of-similar-lines.o print-groups-and-free-memory.o parse-text.o
parse-text.o: parse-text.c parse-text.h $(INCLUDE)
print-groups-and-free-memory.o: print-groups-and-free-memory.c print-groups-and-free-memory.h $(INCLUDE)
find-groups-of-similar-lines.o: find-groups-of-similar-lines.c find-groups-of-similar-lines.h parse-text.h $(INCLUDE)
main.o: main.c print-groups-and-free-memory.h find-groups-of-similar-lines.h parse-text.h $(INCLUDE)

clean:
	rm -f *.o similar_lines

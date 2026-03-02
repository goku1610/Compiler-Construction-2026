# Group Number: 20 | Team Members:
# 2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
# 2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
# 2022B2A70653P Saksham Rohatgi

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SOURCES = \
	driver.c \
	lexer.c \
	parser.c \
	parse_runtime.c \
	grammar_sets.c \
	ascii_text.c

OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run

all: stage1exe

stage1exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: stage1exe
	./stage1exe

clean:
	rm -f stage1exe *.o

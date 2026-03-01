CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
BUILD_DIR = build

SOURCES = \
	app/driver_console.c \
	lexer/stream_lexer.c \
	lexer/keyword_bank.c \
	parser/ll1_frontend.c \
	parser/first_follow.c \
	shared/ascii_text.c

OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean run

all: stage1exe

stage1exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: stage1exe
	./stage1exe

clean:
	rm -rf $(BUILD_DIR) stage1exe *.o

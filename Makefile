CC = gcc
CFLAGS = -Wall -std=c99 -Isrc/libs
LDFLAGS = -lraylib -lm -lpthread -ldl -lX11

SRC := $(wildcard src/*.c src/libs/*.c)
OUT := game

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUT)

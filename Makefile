UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    CC = clang
    CFLAGS = -Wall -std=c99 -Isrc/libs -Isrc
    RAYLIB_CFLAGS = -I/opt/homebrew/include -I/usr/local/include
    RAYLIB_LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -lm -lpthread \
        -framework CoreVideo -framework IOKit -framework Cocoa \
        -framework GLUT -framework OpenGL
else ifeq ($(UNAME_S),Linux)
    CC = gcc
    CFLAGS = -Wall -std=c99 -Isrc/libs -Isrc
    RAYLIB_CFLAGS =
    RAYLIB_LDFLAGS = -lSDL2 -lraylib -lm -ldl -lpthread -lX11
endif

CFLAGS += $(RAYLIB_CFLAGS)
LDFLAGS = $(RAYLIB_LDFLAGS)

SRC := $(wildcard src/*.c src/libs/*.c)
OUT := game

.PHONY: all clean

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OUT)

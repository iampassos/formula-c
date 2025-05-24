CC = gcc
CFLAGS = -Wall -std=c99 -Isrc/libs -Isrc

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    CC = clang

    RAYLIB_PC_DIR := $(shell find /opt/homebrew/opt/raylib/lib/pkgconfig -name "raylib.pc" 2>/dev/null | xargs dirname)
    ifeq ($(strip $(RAYLIB_PC_DIR)),)
        RAYLIB_PC_DIR := $(shell find /usr/local/opt/raylib/lib/pkgconfig -name "raylib.pc" 2>/dev/null | xargs dirname)
    endif

    ifeq ($(strip $(RAYLIB_PC_DIR)),)
        RAYLIB_PKG_CONFIG_PATH_ENV = PKG_CONFIG_PATH=/opt/homebrew/lib/pkgconfig:/usr/local/lib/pkgconfig
    else
        RAYLIB_PKG_CONFIG_PATH_ENV = PKG_CONFIG_PATH=$(RAYLIB_PC_DIR)
    endif

    RAYLIB_CFLAGS_AUTO = $(shell $(RAYLIB_PKG_CONFIG_PATH_ENV) pkg-config --cflags raylib 2>/dev/null || echo -I/opt/homebrew/include -I/usr/local/include)
    RAYLIB_LDFLAGS_AUTO = $(shell $(RAYLIB_PKG_CONFIG_PATH_ENV) pkg-config --libs raylib 2>/dev/null || echo -L/opt/homebrew/lib -L/usr/local/lib -lraylib -lm -lpthread -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL)

    CFLAGS += $(RAYLIB_CFLAGS_AUTO)
    LDFLAGS = $(RAYLIB_LDFLAGS_AUTO)

else ifeq ($(UNAME_S),Linux)
    RAYLIB_CFLAGS_AUTO = $(shell pkg-config --cflags raylib 2>/dev/null || echo "")
    RAYLIB_LDFLAGS_AUTO = $(shell pkg-config --libs raylib 2>/dev/null || echo "-lraylib -lm -lpthread -ldl -lX11")

    CFLAGS += $(RAYLIB_CFLAGS_AUTO)
    LDFLAGS = $(RAYLIB_LDFLAGS_AUTO)

else
    LDFLAGS = -lraylib -lm -lpthread -ldl
endif

SRC := $(wildcard src/*.c src/libs/*.c)
OUT := game

.PHONY: all run clean

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUT)

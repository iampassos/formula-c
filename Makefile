CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -lm -lpthread -ldl -lX11

# Alvo padrão para compilar o código
all: game

# Como compilar o código
game: src/linked_list.c src/car.c src/camera.c src/game.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Como limpar os arquivos compilados
clean:
	rm -f game

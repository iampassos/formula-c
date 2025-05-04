CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -lm -lpthread -ldl -lX11

# Alvo padrão para compilar o código
all: meu_jogo

# Como compilar o código
meu_jogo: meu_jogo.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Como limpar os arquivos compilados
clean:
	rm -f meu_jogo

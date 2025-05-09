#include "car.h"
#include "linked_list.h"
#include "raylib.h"

#define BACKGROUND_COLOR (Color){186, 149, 127, 255}

// Cores das partes da pista
#define TRACK_COLOR (Color){127, 127, 127}
#define LIGHT_ESCAPE_AREA_COLOR (Color){255, 127, 39}
#define HARD_ESCAPE_AREA_COLOR (Color){163, 73, 164}
#define OUTSIDE_TRACK_COLOR (Color){255, 255, 255}

// Forças de atrito do carro com as diferentes partes da pista
#define TRACK_DRAG 0.01
#define LIGHT_ESCAPE_AREA_DRAG 0.04
#define HARD_ESCAPE_AREA_DRAG 0.07
#define OUTSIDE_TRACK_DRAG 0.2

// Largura e altura da tela em pixels
#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

void setup();   // Função para carregar o cenário e variáveis globais
void cleanup(); // Função para liberar os recursos após o fim da execução do
                // jogo
void draw();    // Função que é executada a cada frame

LinkedList *cars; // Variável para armazenar a lista encadeada dos carros da corrida

// Armazenam a imagem que vai ser colocada de plano de fundo
Texture2D trackBackground;

int main() {
    setup();

    while (!WindowShouldClose()) {
        BeginDrawing();
        draw();
        EndDrawing();
    }

    cleanup();

    return 0;
}

void setup() {
    // Variáveis que serão enviadas para a biblioteca car.h processar a interação carro pista
    Color *trackPixels;
    Image  trackMask;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Formula C"); // Inicializando a tela gráfica 2d
    SetTargetFPS(60); // Definindo o frame rate em 60

    trackMask = LoadImage("resources/masks/pista_debug_mask.png");
    ImageResize(&trackMask, SCREEN_WIDTH, SCREEN_HEIGHT);
    trackPixels = LoadImageColors(trackMask);
    

    Image bgImage = LoadImage("resources/masks/pista_debug_mask.png"); // Pista exibida
    ImageResize(&bgImage, SCREEN_WIDTH, SCREEN_HEIGHT);                // redimensiona
    trackBackground = LoadTextureFromImage(bgImage);                   // converte em textura
    UnloadImage(bgImage); // libera o recurso da imagem após virar textura

    Track_setMask(trackMask, trackPixels);
    Track_setDrag(TRACK_DRAG, LIGHT_ESCAPE_AREA_DRAG, HARD_ESCAPE_AREA_DRAG, OUTSIDE_TRACK_DRAG);
    Track_setColor(TRACK_COLOR, LIGHT_ESCAPE_AREA_COLOR, HARD_ESCAPE_AREA_COLOR,
                   OUTSIDE_TRACK_COLOR);

    cars = LinkedList_create();

    Car *car1 = Car_create((Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 3.0f}, // pos
                           0.1,    // aceleracao do carro
                           50,     // largura
                           20,     // altura
                           ORANGE, // Cor
                           0.0,    // angulo inicial do carro
                           0.035,  // aceleraçao angular ( velocidade de rotacao),
                           0.2,    // Velocidade mínima para fazer curva,
                           0.07,   // Força de frenagem,
                           0.2,    // Força da marcha ré
                           1       // id do carro
    );

    LinkedList_addLast(cars, car1); // Adicionando o carro criado no fim da lista encadeada

    Car *car2 = Car_create((Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 3.0f}, // pos
                           0.1,   // aceleracao do carro
                           50,    // largura
                           20,    // altura
                           BLUE,  // Cor
                           0.0,   // angulo inicial do carro
                           0.025, // aceleraçao angular ( velocidade de rotacao),
                           0.2,   // Velocidade mínima para fazer curva
                           0.05,  // Força de frenagem
                           0.2,   // Força da marcha ré
                           2      // id do carro
    );

    LinkedList_addLast(cars, car2); // Adicionando o carro criado no fim da lista encadeada
}

void cleanup() {
    CloseWindow(); // Fechar a janela gráfica 2d
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload(); // função que deve liberar o trackMask e trackPixels
    LinkedList_free(cars); // Libera a memória da lista encadeada de carros
}

void draw() {
    ClearBackground(BACKGROUND_COLOR);
    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    Car *player1 = LinkedList_search(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_move(player1, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT); // Movendo o carro do player 1 de acordo com essas teclas

    Car *player2 = LinkedList_search(cars, 2); // Pegando o carro com id 2 da lista encadeada

    Car_move(player2, KEY_W, KEY_S, KEY_D, KEY_A); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(cars, Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada
    LinkedList_forEach(cars, Car_draw);   // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada
}

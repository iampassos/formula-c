#include "car.h"
#include "linked_list.h"
#include "raylib.h"

#define BACKGROUND_COLOR (Color){186, 149, 127, 255}

#define TRACK_COLOR (Color){127, 127, 127, 255}
#define LIGHT_ESCAPE_AREA_COLOR (Color){255, 127, 39, 255}
#define HARD_ESCAPE_AREA_COLOR (Color){163, 73, 164, 255}
#define OUTSIDE_TRACK_COLOR (Color){255, 255, 255, 255}

#define TRACK_DRAG 0.99
#define LIGHT_ESCAPE_AREA_DRAG 0.96
#define HARD_ESCAPE_AREA_DRAG 0.93
#define OUTSIDE_TRACK_DRAG 0.8

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

void setup();   // Função para carregar o cenário e variáveis globais
void cleanup(); // Função para liberar os recursos após o fim da execução do
                // jogo
void draw();    // Função que é executada a cada frame

LinkedList *cars;

Texture2D trackTex;
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
    Color *trackPixels;
    Image  trackMask;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Formula C");
    SetTargetFPS(60);

    trackMask = LoadImage("resources/masks/pista_debug_mask.png");
    ImageResize(&trackMask, SCREEN_WIDTH, SCREEN_HEIGHT);
    trackPixels = LoadImageColors(trackMask);
    trackTex    = LoadTextureFromImage(trackMask);

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
                           1       // id do carro
    );

    LinkedList_addLast(cars, car1);

    Car *car2 = Car_create((Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 3.0f}, // pos
                           0.1,   // aceleracao do carro
                           50,    // largura
                           20,    // altura
                           BLUE,  // Cor
                           0.0,   // angulo inicial do carro
                           0.025, // aceleraçao angular ( velocidade de rotacao),
                           0.2,   // Velocidade mínima para fazer curva
                           0.05,  // Força de frenagem
                           2      // id do carro
    );

    LinkedList_addLast(cars, car2);
}

void cleanup() {
    CloseWindow();
    UnloadTexture(trackBackground);
    UnloadTexture(trackTex);
    Track_Unload(); // função que deve liberar o trackMask e trackPixels
    LinkedList_free(cars);
}

void draw() {
    ClearBackground(BACKGROUND_COLOR);
    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    Car *player1 = LinkedList_search(cars, 1);

    Car_move(player1, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT);

    Car *player2 = LinkedList_search(cars, 2);

    Car_move(player2, KEY_W, KEY_S, KEY_D, KEY_A);

    LinkedList_forEach(cars, Car_update);
    LinkedList_forEach(cars, Car_draw);
}

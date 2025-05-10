#include "car.h"
#include "linked_list.h"
#include "raylib.h"

// #define BACKGROUND_COLOR (Color){186, 149, 127, 255}

// Cores das partes da pista
#define TRACK_COLOR (Color){127, 127, 127}
#define LIGHT_ESCAPE_AREA_COLOR (Color){255, 127, 39}
#define HARD_ESCAPE_AREA_COLOR (Color){163, 73, 164}
#define OUTSIDE_TRACK_COLOR (Color){255, 255, 255}

// Cores dos checkpoints
#define RACE_START_COLOR (Color){0, 255, 0}
#define FIRST_CHECKPOINT_COLOR (Color){0, 0, 255}
#define SECOND_CHECKPOINT_COLOR (Color){255, 0, 0}

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
void update();  // Função que é executada a cada frame para atualizar o estado do jogo
void draw();    // Função que é executada a cada frame para desenhar o estado do jogo

LinkedList *cars; // Variável para armazenar a lista encadeada dos carros da corrida
Camera2D    camera;

// Armazenam a imagem que vai ser colocada de plano de fundo
Texture2D trackBackground;

int main() {
    setup();

    while (!WindowShouldClose()) {
        BeginDrawing();
        update();
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
    SetTargetFPS(60);                                     // Definindo o frame rate em 60

    // trackMask = LoadImage("resources/masks/pista_debug_mask.png");
    trackMask = LoadImage("resources/maps/interlagos.png");
    // ImageResize(&trackMask, SCREEN_WIDTH, SCREEN_HEIGHT);
    trackPixels = LoadImageColors(trackMask);

    // Image bgImage = LoadImage("resources/masks/pista_debug_mask.png"); // Pista exibida
    Image bgImage = LoadImage("resources/maps/interlagos.png"); // Pista exibida
    // ImageResize(&bgImage, SCREEN_WIDTH, SCREEN_HEIGHT);         // redimensiona
    trackBackground = LoadTextureFromImage(bgImage); // converte em textura
    UnloadImage(bgImage); // libera o recurso da imagem após virar textura

    Track_setMask(trackMask, trackPixels);
    Track_setDrag(TRACK_DRAG, LIGHT_ESCAPE_AREA_DRAG, HARD_ESCAPE_AREA_DRAG, OUTSIDE_TRACK_DRAG);
    Track_setColor(TRACK_COLOR, LIGHT_ESCAPE_AREA_COLOR, HARD_ESCAPE_AREA_COLOR,
                   OUTSIDE_TRACK_COLOR, RACE_START_COLOR, FIRST_CHECKPOINT_COLOR,
                   SECOND_CHECKPOINT_COLOR);

    cars = LinkedList_create();

    Car *car = Car_create((Vector2) {2500, 3600}, // pos
                          0.2,                    // aceleracao do carro
                          75,                     // largura
                          25,                     // altura
                          ORANGE,                 // Cor
                          0.0,                    // angulo inicial do carro
                          0.035,                  // aceleraçao angular ( velocidade de rotacao),
                          0.2,                    // Velocidade mínima para fazer curva,
                          0.07,                   // Força de frenagem,
                          0.2,                    // Força da marcha ré
                          1                       // id do carro
    );

    camera.target   = (Vector2) car->pos;
    camera.offset   = (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom     = 1.5f;

    LinkedList_addLast(cars, car); // Adicionando o carro criado no fim da lista encadeada
}

void cleanup() {
    CloseWindow();                  // Fechar a janela gráfica 2d
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload();                 // função que deve liberar o trackMask e trackPixels
    LinkedList_free(cars);          // Libera a memória da lista encadeada de carros
}

void update() {
    Car *player = LinkedList_search(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_showInfo(player, 100, 100, 20, BLACK);

    Car_move(player, KEY_W, KEY_S, KEY_D,
             KEY_A); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

    camera.target = (Vector2) player->pos;
}

void draw() {
    BeginMode2D(camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();
}

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
void updateCameraTarget(Car *car); // Função que lida com a posição da camera
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Formula C"); // Inicializando a tela gráfica 2d
    SetTargetFPS(60);                                     // Definindo o frame rate em 60

    // trackMask = LoadImage("resources/masks/pista_debug_mask.png");
    Image trackMask = LoadImage("resources/masks/interlagos_mask.png");
    // ImageResize(&trackMask, SCREEN_WIDTH, SCREEN_HEIGHT);

    Track_setMask(trackMask);
    
    UnloadImage(trackMask);

    // Image bgImage = LoadImage("resources/masks/pista_debug_mask.png"); // Pista exibida
    Image bgImage = LoadImage("resources/masks/interlagos_mask.png"); // Pista exibida
    // ImageResize(&bgImage, SCREEN_WIDTH, SCREEN_HEIGHT);         // redimensiona
    trackBackground = LoadTextureFromImage(bgImage); // converte em textura
    UnloadImage(bgImage); // libera o recurso da imagem após virar textura

    
    Track_setDrag(TRACK_DRAG, LIGHT_ESCAPE_AREA_DRAG, HARD_ESCAPE_AREA_DRAG, OUTSIDE_TRACK_DRAG);
    Track_setColor(TRACK_COLOR, LIGHT_ESCAPE_AREA_COLOR, HARD_ESCAPE_AREA_COLOR,
                   OUTSIDE_TRACK_COLOR, RACE_START_COLOR, FIRST_CHECKPOINT_COLOR,
                   SECOND_CHECKPOINT_COLOR);

    cars = LinkedList_create();

    Car *car = Car_create((Vector2) {2500, 3600}, // pos
                          0.28,                    // aceleracao do carro
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

    LinkedList_addCar(cars, car); // Adicionando o carro criado na lista encadeada

    camera.target   = car->pos;
    camera.offset   = (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom     = 0.5f;
}

void cleanup() {
    CloseWindow();                  // Fechar a janela gráfica 2d
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload();                 // função que deve liberar o trackMask e trackPixels
    LinkedList_free(cars);          // Libera a memória da lista encadeada de carros
}

void updateCameraTarget(Car *car) {
    const int mapWidth = trackBackground.width;
    const int mapHeight = trackBackground.height;

    float halfW = SCREEN_WIDTH / (2.0f * camera.zoom);
    float halfH = SCREEN_HEIGHT / (2.0f * camera.zoom);

    float x = car->pos.x;
    float y = car->pos.y;

    if (x < halfW) x = halfW;
    if (y < halfH) y = halfH;
    if (x > mapWidth - halfW) x = mapWidth - halfW;
    if (y > mapHeight - halfH) y = mapHeight - halfH;

    camera.target = (Vector2){x, y};
}

void update() {
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_move(player, KEY_W, KEY_S, KEY_D,
             KEY_A); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

    camera.target = player->pos;
    //updateCameraTarget(player);
}

void draw() {
    BeginMode2D(camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_showInfo(player, player->pos.x-500, player->pos.y-500, 30, BLACK);

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();
}

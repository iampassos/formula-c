#include "camera.h"
#include "car.h"
#include "linked_list.h"
#include "raylib.h"

// #define BACKGROUND_COLOR (Color){186, 149, 127, 255}

// Areas da pista
#define TRACK_AREAS                                                                                \
    (TrackArea[]) {                                                                                \
        /* pista */ {(Color) {127, 127, 127}, 0.01}, /* fraca */ {(Color) {255, 127, 39}, 0.04},   \
            /* forte */ {(Color) {163, 73, 164}, 0.07}, /* grama */ {(Color) {34, 177, 76}, 0.02}  \
    }

// Cor que representa fora da pista
#define OUTSIDE_TRACK_COLOR (Color){255, 255, 255}

// Checkpoints
#define CHECKPOINTS                                                                                \
    (Checkpoint[]) {                                                                               \
        {(Color) {0, 255, 0}, (Vector2) {4371, 2537}, 2.66f},                                      \
            {(Color) {0, 0, 255}, (Vector2) {6700, 8147}, 0.0f},                                   \
            {(Color) {255, 0, 0}, (Vector2) {11069, 2257}, 2.17f}                                  \
    }

// Largura e altura da tela em pixels
static int SCREEN_WIDTH;
static int SCREEN_HEIGHT;

void setup();   // Função para carregar o cenário e variáveis globais
void cleanup(); // Função para liberar os recursos após o fim da execução do
                // jogo
void update();  // Função que é executada a cada frame para atualizar o estado do jogo
void draw();    // Função que é executada a cada frame para desenhar o estado do jogo

LinkedList *cars; // Variável para armazenar a lista encadeada dos carros da corrida
Camera2D   *camera;

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
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0),
               "Formula C"); // Inicializando a tela gráfica 2d
    SCREEN_WIDTH  = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    Image icon = LoadImage("resources/logo/formula_c-logo.png");
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);

    trackBackground =
        LoadTexture("resources/masks/interlagos_maskV2.png"); // converte em textura// Definindo o
                                                            // frame rate em 60

    // Carregando a imagem da máscara de pixels
    Track_setMask("resources/masks/interlagos_maskV2.png");
    Track_setAreas(TRACK_AREAS, 4);
    Track_setCheckpoints(CHECKPOINTS, 3);
    Track_setOutsideColor(OUTSIDE_TRACK_COLOR);

    Camera_Screen_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera_Background_setSize(trackBackground.width, trackBackground.height);

    cars = LinkedList_create();

    Car *player = Car_create((Vector2) {5400, 2000}, // pos
                             2.66,                   // angulo inicial do carro

                             0.3,  // aceleracao do carro
                             0.2,  // força da marcha ré
                             0.02, // força de frenagem

                             0.035, // aceleração angular (velocidade de rotação)
                             0.2,   // velocidade mínima para fazer curva

                             125, // largura
                             75,  // altura

                             "resources/cars/carroazul.png", // path da textura
                             1                               // id do carro
    );

    LinkedList_addCar(cars, player); // Adicionando o carro criado na lista encadeada

    camera = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 0.0f,
                           0.5f);
}

void cleanup() {
    CloseWindow();                  // Fechar a janela gráfica 2d
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload();                 // função que deve liberar o trackMask e trackPixels
    Camera_free(camera);
    LinkedList_free(cars); // Libera a memória da lista encadeada de carros
}

void update() {
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_move(player, KEY_W, KEY_S, KEY_D,
             KEY_A); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

    Camera_updateTarget(camera, player); // Atualizando a posição da camera
}

void draw() {
    BeginMode2D(*camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_showInfo(player, player->pos.x - (SCREEN_WIDTH / 2), player->pos.y - (SCREEN_HEIGHT / 2),
                 50,
                 BLACK); // Mostrando as informações do carro com id 1

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();
}
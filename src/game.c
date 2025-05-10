#include "car.h"
#include "linked_list.h"
#include "camera.h"
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Formula C"); // Inicializando a tela gráfica 2d
    SetTargetFPS(60);                    
    
    trackBackground = LoadTexture("resources/masks/interlagos_mask.png"); // converte em textura// Definindo o frame rate em 60

    // Carregando a imagem da máscara de pixels
    Track_setMask("resources/masks/interlagos_mask.png");
    Track_setDrag(TRACK_DRAG, LIGHT_ESCAPE_AREA_DRAG, HARD_ESCAPE_AREA_DRAG, OUTSIDE_TRACK_DRAG);
    Track_setDragColor(TRACK_COLOR, LIGHT_ESCAPE_AREA_COLOR, HARD_ESCAPE_AREA_COLOR,
                   OUTSIDE_TRACK_COLOR);
    Track_setCheckpointColor(RACE_START_COLOR, FIRST_CHECKPOINT_COLOR,SECOND_CHECKPOINT_COLOR);

    Camera_Screen_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera_Background_setSize(trackBackground.width, trackBackground.height);

    cars = LinkedList_create();

    Car *car = Car_create(
        (Vector2){5400, 2000},     // pos
        2.66,                      // angulo inicial do carro

        0.3,                       // aceleracao do carro
        0.2,                       // força da marcha ré
        0.02,                      // força de frenagem

        0.035,                     // aceleração angular (velocidade de rotação)
        0.2,                       // velocidade mínima para fazer curva

        100,                       // largura
        50,                        // altura

        "resources/cars/carroazul.png", // path da textura
        1                          // id do carro
    );

    LinkedList_addCar(cars, car); // Adicionando o carro criado na lista encadeada

    camera = Camera_create(
        car->pos,
        (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
        0.0f,
        0.5f
    );
}

void cleanup() {
    CloseWindow();                  // Fechar a janela gráfica 2d
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload();                 // função que deve liberar o trackMask e trackPixels
    Camera_free(camera);
    LinkedList_free(cars);          // Libera a memória da lista encadeada de carros
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

    Car_showInfo(player, player->pos.x-(SCREEN_WIDTH / 2), player->pos.y-(SCREEN_HEIGHT / 2), 50, BLACK); // Mostrando as informações do carro com id 1

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();
}
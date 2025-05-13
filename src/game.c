#include "game.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

Texture2D   trackBackground; // Armazenam a imagem que vai ser colocada de plano de fundo
LinkedList *cars;            // Variável para armazenar a lista encadeada dos carros da corrida
Camera2D   *camera;

typedef struct {
    Vector2 pos;
    float   angle;
} GhostCarFrame;

GhostCarFrame *best_lap         = NULL;
int            best_lap_i       = 0;
int            best_lap_current = 0;

GhostCarFrame *current_lap   = NULL;
int            current_lap_i = 0;

void load_map(Map map) {
    switch (map) {
    case INTERLAGOS:
        trackBackground =
            LoadTexture("resources/masks/interlagos_maskV2.png"); // converte em textura// Definindo
                                                                  // o frame rate em 60
        // Carregando a imagem da máscara de pixels
        Track_setMask("resources/masks/interlagos_maskV2.png");
        Track_setAreas(TRACK_AREAS, 4);
        Track_setCheckpoints(CHECKPOINTS, 3);
        Track_setOutsideColor(OUTSIDE_TRACK_COLOR);

        state.map = INTERLAGOS;
        break;
    }

    Camera_Screen_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera_Background_setSize(trackBackground.width, trackBackground.height);
}

void setup_game(Mode mode) {
    load_map(INTERLAGOS);

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

    switch (mode) {
    case SINGLEPLAYER:
        LinkedList_addCar(cars, player); // Adicionando o carro criado na lista encadeada
        camera = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
                               0.0f, 0.5f);
        current_lap      = malloc(sizeof(GhostCarFrame));
        best_lap_i       = 0;
        best_lap_current = 0;
        current_lap      = 0;
        break;
    case SPLITSCREEN:
        break;
    }
}

void cleanup_game() {
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload();                 // função que deve liberar o trackMask e trackPixels
    Camera_free(camera);
    LinkedList_free(cars); // Libera a memória da lista encadeada de carros
    free(best_lap);
    free(current_lap);
}

int last_lap = -1;

void update_game() {
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    if (last_lap != player->lap && player->lap >= 1) {
        if (best_lap == NULL || best_lap_i > current_lap_i) {
            free(best_lap);
            best_lap_i = current_lap_i;
            best_lap   = malloc(sizeof(GhostCarFrame) * best_lap_i);
            memcpy(best_lap, current_lap, sizeof(GhostCarFrame) * current_lap_i);
        }

        Car *ghost = LinkedList_getCarById(cars, 99);
        if (ghost) {
            ghost->pos   = best_lap[0].pos;
            ghost->angle = best_lap[0].angle;
        } else {
            Car *new = Car_create(best_lap[0].pos, best_lap[0].angle, 0.3, 0.2, 0.02, 0.035, 0.2,
                                  125, 75, "resources/cars/carroazul.png", 99);
            LinkedList_addCar(cars, new);
        }

        current_lap   = realloc(current_lap, sizeof(GhostCarFrame));
        current_lap_i = 0;

        best_lap_current = 0;
        last_lap         = player->lap;
    }

    Car_move(player, KEY_W, KEY_S, KEY_D, KEY_A,
             KEY_Q); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

    Car *ghost_car = LinkedList_getCarById(cars, 99);
    if (ghost_car) {
        if (best_lap_i > best_lap_current) {
            ghost_car->pos   = best_lap[best_lap_current].pos;
            ghost_car->angle = best_lap[best_lap_current].angle;
            best_lap_current++;
        } else {
            ghost_car->pos = (Vector2) {0, 0};
        }
    }

    if (player->lap >= 0) {
        current_lap = realloc(current_lap, (current_lap_i + 1) * sizeof(GhostCarFrame));
        current_lap[current_lap_i].pos   = player->pos;
        current_lap[current_lap_i].angle = player->angle;
        current_lap_i++;
    }

    Camera_updateTarget(camera, player); // Atualizando a posição da camera
}

void draw_game() {
    BeginMode2D(*camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_showInfo(player, player->pos.x - (SCREEN_WIDTH / 2.0f),
                 player->pos.y - (SCREEN_HEIGHT / 2.0f), 50,
                 BLACK); // Mostrando as informações do carro com id 1

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();

    DrawText("Pressione Q para voltar ao menu", 10, 10, 20, BLACK);

    // Debug ghost car
    char stateText[1000];
    sprintf(stateText, "Ghost car debug:\nRecording i: %d\nPlayback i: %d", best_lap_i,
            best_lap_current);
    DrawText(stateText, 10, 110, 20, BLACK);

    char stateText2[1000];
    sprintf(stateText2, "Current lap debug:\nRecording i: %d", current_lap_i);
    DrawText(stateText2, 10, 200, 20, BLACK);
}

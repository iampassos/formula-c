#include "game.h"
#include "arrayList.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"
#include "raylib.h"
#include "stdio.h"

static Texture2D   trackBackground; // Armazenam a imagem que vai ser colocada de plano de fundo
static LinkedList *cars; // Variável para armazenar a lista encadeada dos carros da corrida
static Camera2D   *camera;

static ArrayList *bestLap    = NULL;
static ArrayList *currentLap = NULL;

static int lastLap        = 0;
static int replayFrameIdx = 0;

static Music music;
static Music carSound;

static void loadMap(Map map) {
    trackBackground = LoadTexture(map.backgroundPath); // converte em textura

    // Carregando a imagem da máscara de pixels
    Track_setMask(map.maskPath);
    Track_setCheckpoints(map.checkpoints);

    Camera_Background_setSize(trackBackground.width, trackBackground.height);

    music    = LoadMusicStream(gameMusicPath);
    carSound = LoadMusicStream(carSoundPath);
    SetMusicVolume(carSound, carVolume);
    SetMusicVolume(music, gameMusicVolume);
    PlayMusicStream(music);
    PlayMusicStream(carSound);
}

void loadSingleplayer() {
    replayFrameIdx  = 0;
    lastLap         = 0;
    bestLap         = ArrayList_create();
    bestLap->length = -1;
    currentLap      = ArrayList_create();
    Car *ghostCar   = Car_create((Vector2) {0, 0}, 2.66, 0.3, 0.2, 0.02, 0.035, 0.2, 150, 75,
                                 "resources/cars/carroazul.png", WHITE, 1, 99);
    Car *player     = Car_create((Vector2) {5400, 2000}, // pos
                                 2.66,                   // angulo inicial do carro

                                 0.3,  // aceleracao do carro
                                 0.2,  // força da marcha ré
                                 0.02, // força de frenagem

                                 0.035, // aceleração angular (velocidade de rotação)
                                 0.2,   // velocidade mínima para fazer curva

                                 150, // largura
                                 75,  // altura

                                 "resources/cars/carroazul.png", // path da textura
                                 WHITE, 0,
                                 1 // id do carro
        );
    LinkedList_addCar(cars, ghostCar);
    LinkedList_addCar(cars, player); // Adicionando o carro criado na lista encadeada
    camera = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 0.0f,
                           0.5f);
}

void setup_game(Mode mode, Map map) {
    loadMap(map);
    cars = LinkedList_create();

    switch (mode) {
    case SINGLEPLAYER:
        loadSingleplayer();
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
    ArrayList_free(bestLap);
    ArrayList_free(currentLap);
    UnloadMusicStream(music);
    UnloadMusicStream(carSound);
}

static void update_ghost_car(Car *player) {
    Car *ghost = LinkedList_getCarById(cars, 99);
    if (player->lap > lastLap) {
        lastLap        = player->lap;
        replayFrameIdx = 0;
        ArrayList_push(currentLap, (GhostCarFrame) {(Vector2) {0, 0}, 0});
        if (ArrayList_length(currentLap) < ArrayList_length(bestLap)) {
            ArrayList_copy(bestLap, currentLap);
        }
        ArrayList_clear(currentLap);
    }

    if (lastLap >= 1) { // Replay
        if (replayFrameIdx < ArrayList_length(bestLap)) {
            GhostCarFrame frameData = ArrayList_get(bestLap, replayFrameIdx++);
            ghost->pos              = frameData.pos;
            ghost->angle            = frameData.angle;
        }
    }

    if (player->lap >= 0) { // Grava
        GhostCarFrame frameData = {player->pos, player->angle};
        ArrayList_push(currentLap, frameData);
    }
}

void update_game() {
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    SetMusicPitch(carSound, 0.6 + player->vel / 13.0f);
    SetMusicVolume(music, 0.2);
    UpdateMusicStream(music);
    UpdateMusicStream(carSound);

    update_ghost_car(player);

    Car_move(player, KEY_W, KEY_S, KEY_D, KEY_A,
             KEY_Q); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

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
    sprintf(stateText, "Ghost car debug:\nRecording i: %u\nPlayback i: %u",
            ArrayList_length(bestLap), replayFrameIdx);
    DrawText(stateText, 10, 110, 20, BLACK);

    char stateText2[1000];
    sprintf(stateText2, "Current lap debug:\nRecording i: %d", ArrayList_length(currentLap));
    DrawText(stateText2, 10, 200, 20, BLACK);
}

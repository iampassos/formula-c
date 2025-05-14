#include "game.h"
#include "arrayList.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"
#include "menu.h"
#include "raylib.h"
#include "stdio.h"

static Texture2D   trackBackground; // Armazenam a imagem que vai ser colocada de plano de fundo
static Texture2D   trackHudBackground;
static LinkedList *cars; // Variável para armazenar a lista encadeada dos carros da corrida
static Camera2D   *camera;

static int minimapWidth;
static int minimapHeigth;

static ArrayList *bestLap    = NULL;
static ArrayList *currentLap = NULL;

static int lastLap        = 0;
static int replayFrameIdx = 0;

static Music music;
static Music carSound;

static Map actualMap;

static void loadMap(Map map) {
    actualMap       = map;
    trackBackground = LoadTexture(map.backgroundPath); // converte em textura

    Image minimap = LoadImage(map.backgroundPath); // Carrega a imagem do arquivo
    minimapWidth  = SCREEN_WIDTH / 4;
    minimapHeigth = SCREEN_HEIGHT / 4;
    ImageResize(&minimap, minimapWidth, minimapHeigth); // Redimensiona a imagem
    trackHudBackground = LoadTextureFromImage(minimap); // Converte a imagem em textura
    UnloadImage(minimap);

    // Carregando a imagem da máscara de pixels
    Track_setMask(map.maskPath);
    Track_setCheckpoints(map.checkpoints);

    Camera_Background_setSize(trackBackground.width, trackBackground.height);

    music    = LoadMusicStream(GAME_MUSIC_PATH);
    carSound = LoadMusicStream(CAR_SOUND_PATH);
    SetMusicVolume(carSound, CAR_VOLUME);
    SetMusicVolume(music, GAME_MUSIC_VOLUME);
    PlayMusicStream(music);
    PlayMusicStream(carSound);
}

void Game_loadSingleplayer(Map map) {
    loadMap(map);
    replayFrameIdx  = 0;
    lastLap         = 0;
    bestLap         = ArrayList_create();
    bestLap->length = -1;
    currentLap      = ArrayList_create();
    Car *ghostCar = Car_create((Vector2) {-1000, -1000}, 2.66, 0.3, 0.2, 0.02, 0.035, 0.2, 150, 75,
                               "resources/cars/carroazul.png", WHITE, 1, 99);
    Car *player   = Car_create(actualMap.startCarPos, // pos
                               actualMap.startAngle,  // angulo inicial do carro

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

static void updateGhostCar(Car *player) {
    Car *ghost = LinkedList_getCarById(cars, 99);
    if (player->lap > lastLap) {
        lastLap        = player->lap;
        replayFrameIdx = 0;
        ArrayList_push(currentLap, (GhostCarFrame) {(Vector2) {-1000, -1000}, 0});
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

void Game_setup() {
    cars = LinkedList_create();
}

void Game_map_cleanup() {
    Track_Unload();
    LinkedList_clear(cars);
    UnloadTexture(trackBackground);    // Liberando a textura da imagem do plano de fundo
    UnloadTexture(trackHudBackground); // Liberando a textura da imagem do plano de fundo
    Camera_free(camera);
    ArrayList_free(bestLap);
    ArrayList_free(currentLap);
    UnloadMusicStream(music);
    UnloadMusicStream(carSound);
}

void Game_cleanup() {
    LinkedList_free(cars); // Libera a memória da lista encadeada de carros
}

void Game_update() {
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    SetMusicPitch(carSound, 0.6 + player->vel / 13.0f);
    SetMusicVolume(music, 0.2);
    UpdateMusicStream(music);
    UpdateMusicStream(carSound);

    updateGhostCar(player);

    Car_move(player, KEY_W, KEY_S, KEY_D,
             KEY_A); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

    Camera_updateTarget(camera, player); // Atualizando a posição da camera
}

static void drawHud() {
    Car *ghost  = LinkedList_getCarById(cars, 99);
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    // Mostrando as informações do carro com id 1
    Car_showInfo(player, SCREEN_WIDTH - 400, 300, 20, BLACK);
    DrawText("Pressione Q para voltar ao menu", SCREEN_WIDTH / 2, 10, 20, BLACK);

    // Debug ghost car
    char stateText[1000];
    sprintf(stateText, "Ghost car debug:\nRecording i: %u\nPlayback i: %u",
            ArrayList_length(bestLap), replayFrameIdx);
    DrawText(stateText, 10, 500, 20, BLACK);

    char stateText2[1000];
    sprintf(stateText2, "Current lap debug:\nRecording i: %d", ArrayList_length(currentLap));
    DrawText(stateText2, 10, 600, 20, BLACK);

    DrawTexture(trackHudBackground, 0, 0, (Color){255, 255, 255, 200});
    float xPlayerHud = trackHudBackground.width * player->pos.x / trackBackground.width;
    float yPlayerHud = trackHudBackground.height * player->pos.y / trackBackground.height;

    float xGhostHud = trackHudBackground.width * ghost->pos.x / trackBackground.width;
    float yGhostHud = trackHudBackground.height * ghost->pos.y / trackBackground.height;

    DrawCircle(xPlayerHud, yPlayerHud, 8, RED);
    DrawCircle(xGhostHud, yGhostHud, 8, GREEN);
}

void Game_draw() {
    BeginMode2D(*camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();

    drawHud();

    if (IsKeyDown(KEY_Q)) {
        Menu_reset();
        state.screen = MENU;
        Game_map_cleanup();
    }
}
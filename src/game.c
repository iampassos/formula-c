#include "game.h"
#include "arrayList.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"
#include "raylib.h"
#include "stdio.h"
#include <math.h>
#include <stdio.h>

static Texture2D   trackBackground; // Armazenam a imagem que vai ser colocada de plano de fundo
static Texture2D   trackHud;
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

static void loadMap(Map map) {
    trackBackground = LoadTexture(map.backgroundPath); // converte em textura

    Image minimap = LoadImage(map.backgroundPath); // Carrega a imagem do arquivo
    minimapWidth  = SCREEN_WIDTH / 4;
    minimapHeigth = SCREEN_HEIGHT / 4;
    ImageResize(&minimap, minimapWidth, minimapHeigth); // Redimensiona a imagem
    trackHud = LoadTextureFromImage(minimap);           // Converte a imagem em textura
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

static void update_best_lap() {
    FILE *file = fopen("./data/best_lap.bin", "wb");
    if (file != NULL) {
        for (int i = 0; i < bestLap->length; i++) {
            fwrite(&bestLap->data[i], sizeof(GhostCarFrame), 1, file);
        }
        fclose(file);
    }
}

static void load_best_lap() {
    FILE *file = fopen("./data/best_lap.bin", "rb");
    if (file != NULL) {
        GhostCarFrame buffer;
        while (fread(&buffer, sizeof(GhostCarFrame), 1, file) == 1) {
            ArrayList_push(bestLap, buffer);
        }
        fclose(file);
    }
}

void Game_loadSingleplayer() {
    state.mode   = SINGLEPLAYER;
    state.screen = GAME;
    Map map      = MAPS[SELECTED_MAP_IDX];
    loadMap(map);
    replayFrameIdx = 0;
    lastLap        = 0;
    bestLap        = ArrayList_create();
    load_best_lap();
    currentLap    = ArrayList_create();
    Car *ghostCar = Car_create((Vector2) {-1000, -1000}, 2.66, 0.3, 0.2, 0.02, 0.035, 0.2, 150, 75,
                               CAR_IMAGE_PATH, WHITE, true, 99);
    Car *player   = Car_create(map.startCarPos, // pos
                               map.startAngle,  // angulo inicial do carro

                               0.3,  // aceleracao do carro
                               0.2,  // força da marcha ré
                               0.02, // força de frenagem

                               0.035, // aceleração angular (velocidade de rotação)
                               0.2,   // velocidade mínima para fazer curva

                               150, // largura
                               75,  // altura

                               CAR_IMAGE_PATH, // path da textura
                               WHITE,          // Cor do carro
                               false,          // Carro fantasma
                               1               // id do carro
      );
    LinkedList_addCar(cars, ghostCar);
    LinkedList_addCar(cars, player); // Adicionando o carro criado na lista encadeada
    camera = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 0.0f,
                           0.5f);
}

void Game_loadSplitscreen() {
    return;
}

static void updateGhostCar(Car *player) {
    Car *ghost = LinkedList_getCarById(cars, 99);
    if (player->lap > lastLap) {
        lastLap        = player->lap;
        replayFrameIdx = 0;
        ArrayList_push(currentLap, (GhostCarFrame) {(Vector2) {-1000, -1000}, 0});
        printf("%d %d\n", ArrayList_length(currentLap), ArrayList_length(bestLap));
        if (ArrayList_length(currentLap) < ArrayList_length(bestLap) ||
            ArrayList_length(bestLap) == 0) {
            ArrayList_copy(bestLap, currentLap);
            update_best_lap();
        }
        ArrayList_clear(currentLap);
    }

    if (player->lap >= 0) {
        // Replay
        if (replayFrameIdx < ArrayList_length(bestLap)) {
            GhostCarFrame frameData = ArrayList_get(bestLap, replayFrameIdx++);
            ghost->pos              = frameData.pos;
            ghost->angle            = frameData.angle;
        }

        // Grava
        GhostCarFrame frameData = {player->pos, player->angle};
        ArrayList_push(currentLap, frameData);
    }
}

void Game_setup() {
    cars = LinkedList_create();
}

static void mapCleanup() {
    Track_Unload();
    LinkedList_clear(cars);
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    UnloadTexture(trackHud);        // Liberando a textura da imagem do plano de fundo
    Camera_free(camera);
    ArrayList_free(bestLap);
    ArrayList_free(currentLap);
    UnloadMusicStream(music);
    UnloadMusicStream(carSound);
}

void Game_cleanup() {
    // Se o usuário fechou o jogo em outra tela além do jogo, limpar a memoria do jogo
    if (state.screen != GAME)
        mapCleanup();
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

static void drawMinimap(Car *player, Car *ghost) {
    float textureX = SCREEN_WIDTH - trackHud.width;

    DrawTexture(trackHud, textureX, 0, (Color) {255, 255, 255, HUD_OPACITY});
    float xPlayerHud = trackHud.width * player->pos.x / trackBackground.width + textureX;
    float yPlayerHud = trackHud.height * player->pos.y / trackBackground.height;

    float xGhostHud = trackHud.width * ghost->pos.x / trackBackground.width + textureX;
    float yGhostHud = trackHud.height * ghost->pos.y / trackBackground.height;

    DrawCircle(xPlayerHud, yPlayerHud, 8, RED);
    DrawCircle(xGhostHud, yGhostHud, 8, GREEN);
}

static void drawSpeedometer(Car *player) {
    float speedometerAngle = -PI + PI * player->vel / player->maxVelocity;
    float speedometerSize  = SCREEN_WIDTH / 12;

    Vector2 speedometerStart = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - SCREEN_HEIGHT / 60};
    Vector2 speedometerEnd   = {cosf(speedometerAngle) * speedometerSize + speedometerStart.x,
                                sinf(speedometerAngle) * speedometerSize + speedometerStart.y};

    DrawCircleSector(speedometerStart, speedometerSize, -180, 0, 32,
                     (Color) {0, 0, 0, HUD_OPACITY});
    DrawCircleSectorLines(speedometerStart, speedometerSize, -180, 0, 32, WHITE);

    int   tickCount  = 9; // número de marcações, por exemplo de 0 a 100 em passos de 10
    float tickLength = 10.0f;

    for (int i = 0; i <= tickCount; i++) {
        float t     = (float) i / tickCount;
        float angle = -PI + PI * t; // de -π a 0
        float cosA  = cosf(angle);
        float sinA  = sinf(angle);

        Vector2 start = {speedometerStart.x + cosA * (speedometerSize - tickLength),
                         speedometerStart.y + sinA * (speedometerSize - tickLength)};

        Vector2 end = {speedometerStart.x + cosA * speedometerSize,
                       speedometerStart.y + sinA * speedometerSize};

        DrawLineEx(start, end, 2, (Color) {255, 255, 255, HUD_OPACITY});

        int     velValue = (int) (player->maxVelocity * t);
        Vector2 labelPos = {speedometerStart.x + cosA * (speedometerSize - tickLength - 15),
                            speedometerStart.y + sinA * (speedometerSize - tickLength - 15)};
        DrawText(TextFormat("%d", velValue), (int) labelPos.x - 10, (int) labelPos.y - 10, 14,
                 WHITE);
    }

    float t           = player->vel / player->maxVelocity;
    Color needleColor = (Color) {(unsigned char) (255 * t),       // vermelho aumenta
                                 (unsigned char) (255 * (1 - t)), // verde diminui
                                 0, HUD_OPACITY};

    DrawLineEx(speedometerStart, speedometerEnd, 5, needleColor);
}

static void drawDebugInfo(Car *player, Car *ghost) {
    // Mostrando as informações do carro com id 1
    Car_showInfo(player, SCREEN_WIDTH - 400, 300, 20, BLACK);
    // Debug ghost car
    char stateText[1000];
    sprintf(stateText, "Ghost car debug:\nRecording i: %u\nPlayback i: %u",
            ArrayList_length(bestLap), replayFrameIdx);
    DrawText(stateText, 10, 500, 20, BLACK);

    char stateText2[1000];
    sprintf(stateText2, "Current lap debug:\nRecording i: %d", ArrayList_length(currentLap));
    DrawText(stateText2, 10, 600, 20, BLACK);
}

static void drawHud() {
    Car *ghost  = LinkedList_getCarById(cars, 99);
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    DrawText("Pressione Q para voltar ao menu", 10, 10, 20, BLACK);

    drawDebugInfo(player, ghost);
    drawMinimap(player, ghost);
    drawSpeedometer(player);
}

void Game_draw() {
    BeginMode2D(*camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();

    drawHud();

    if (IsKeyDown(KEY_Q)) {
        state.screen = MENU;
        mapCleanup();
    }
}

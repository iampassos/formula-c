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
#include <string.h>

static Texture2D   trackBackground; // Armazenam a imagem que vai ser colocada de plano de fundo
static Texture2D   trackHud;
static LinkedList *cars; // Variável para armazenar a lista encadeada dos carros da corrida

static Camera2D *camera1;
static Camera2D *camera2;

static char ghostCarPath[100];

static int minimapWidth;
static int minimapHeigth;

static ArrayList *bestLap    = NULL;
static ArrayList *currentLap = NULL;

static int lastLap        = 0;
static int replayFrameIdx = 0;

static Music music;
static Music carSound;

static void loadMap(Map map) {
    // Carrega a imagem de fundo
    trackBackground = LoadTexture(state.debug ? map.maskPath : map.backgroundPath);

    // Carrega a imagem do minimapa
    Image minimap = LoadImage(state.debug ? map.maskPath : map.minimapPath);
    minimapWidth  = SCREEN_WIDTH / 4;
    minimapHeigth = SCREEN_HEIGHT / 4;
    ImageResize(&minimap, minimapWidth, minimapHeigth); // Redimensiona a imagem
    trackHud = LoadTextureFromImage(minimap);           // Converte a imagem em textura
    UnloadImage(minimap);

    // Carregando a imagem da máscara de pixels
    Track_setMask(map.maskPath);
    Track_setCheckpoints(map.checkpoints, map.checkpointSize);

    Camera_Background_setSize(trackBackground.width, trackBackground.height);

    music    = LoadMusicStream(GAME_MUSIC_PATH);
    carSound = LoadMusicStream(CAR_SOUND_PATH);
    SetMusicVolume(carSound, CAR_VOLUME);
    SetMusicVolume(music, GAME_MUSIC_VOLUME);
    PlayMusicStream(music);
    PlayMusicStream(carSound);
}

static void update_best_lap() {
    FILE *file = fopen(ghostCarPath, "wb");
    if (file != NULL) {
        for (int i = 0; i < bestLap->length; i++) {
            fwrite(&bestLap->data[i], sizeof(GhostCarFrame), 1, file);
        }
        fclose(file);
    }
}

static void load_best_lap() {
    FILE *file = fopen(ghostCarPath, "rb");
    if (file != NULL) {
        GhostCarFrame buffer;
        while (fread(&buffer, sizeof(GhostCarFrame), 1, file) == 1) {
            ArrayList_push(bestLap, buffer);
        }
        fclose(file);
    }
}

static void loadSingleplayer(Map map) {
    strcpy(ghostCarPath, GHOST_CAR_DATA_PATH);
    strcat(ghostCarPath, map.name);
    strcat(ghostCarPath, ".bin");

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
                               0.5,   // velocidade mínima para fazer curva

                               150, // largura
                               75,  // altura

                               CAR_IMAGE_PATH, // path da textura
                               WHITE,          // Cor do carro
                               false,          // Carro fantasma
                               1               // id do carro
      );
    LinkedList_addCar(cars, ghostCar);
    LinkedList_addCar(cars, player); // Adicionando o carro criado na lista encadeada
    camera1 = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
                            0.0f, 0.5f);
}

static void loadSplitscreen(Map map) {
    Car *p1 = Car_create(map.startCarPos, map.startAngle, 0.3, 0.2, 0.02, 0.035, 0.5, 150, 75,
                         CAR_IMAGE_PATH, RED, false, 1);

    Car *p2 = Car_create(map.startCarPos, map.startAngle, 0.3, 0.2, 0.02, 0.035, 0.5, 150, 75,
                         CAR_IMAGE_PATH, BLUE, false, 2);

    LinkedList_addCar(cars, p1);
    LinkedList_addCar(cars, p2);

    camera1 =
        Camera_create(p1->pos, (Vector2) {SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f}, 0.0f, 0.5f);
    camera2 = Camera_create(p2->pos, (Vector2) {SCREEN_WIDTH * 3.0f / 4.0f, SCREEN_HEIGHT / 2.0f},
                            0.0f, 0.5f);
}

void Game_load() {
    state.screen = GAME;
    Map map      = MAPS[state.map];
    loadMap(map);
    switch (state.mode) {
    case SINGLEPLAYER:
        loadSingleplayer(map);
        break;
    case SPLITSCREEN:
        loadSplitscreen(map);
        break;
    }
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
    Camera_free(camera1);
    Camera_free(camera2);
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
    Car *p1 = LinkedList_getCarById(cars, 1);

    if (state.mode == SINGLEPLAYER) {
        SetMusicPitch(carSound, 0.6 + p1->vel / 13.0f);
        SetMusicVolume(music, GAME_MUSIC_VOLUME);
        UpdateMusicStream(music);
        UpdateMusicStream(carSound);

        updateGhostCar(p1);

        Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);

        LinkedList_forEach(cars, Car_update);

        Camera_updateTarget(camera1, p1);
    } else {
        Car *p2 = LinkedList_getCarById(cars, 2);

        Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);
        Car_move(p2, KEY_I, KEY_K, KEY_L, KEY_J);

        LinkedList_forEach(cars, Car_update);

        Camera_updateTarget(camera1, p1);
        Camera_updateTarget(camera2, p2);
    }
}

static void drawDebugInfo(Car *player, Car *ghost) {
    Car_showInfo(player, SCREEN_WIDTH - 400, 300, 20, BLACK);

    char stateText[1000];
    sprintf(stateText, "Ghost car debug:\nRecording i: %u\nPlayback i: %u",
            ArrayList_length(bestLap), replayFrameIdx);
    DrawText(stateText, 10, 500, 20, BLACK);

    char stateText2[1000];
    sprintf(stateText2, "Current lap debug:\nRecording i: %d", ArrayList_length(currentLap));
    DrawText(stateText2, 10, 600, 20, BLACK);
}

static void drawMinimap(Car *player) {
    float x =
        trackHud.width * player->pos.x / trackBackground.width + SCREEN_WIDTH - trackHud.width;
    float y = trackHud.height * player->pos.y / trackBackground.height;

    if (player->ghost) {
        DrawCircleLines(x, y, 3.5f, BLACK);
        DrawCircle(x, y, 3, WHITE);
    } else {
        DrawCircleLines(x, y, 6.5f, BLACK);
        DrawCircle(x, y, 6, player->color);
    }
}

static void drawHud() {
    DrawText("Pressione Q para voltar ao menu", 10, 10, 20, BLACK);

    if (state.debug) {
        Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

        Car *ghost = LinkedList_getCarById(cars, 99);
        drawDebugInfo(player, ghost);
    }

    DrawTexture(trackHud, SCREEN_WIDTH - trackHud.width, 0, (Color) {255, 255, 255, HUD_OPACITY});
    LinkedList_forEach(cars, drawMinimap);
}

void Map_draw() {
    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo
    LinkedList_forEach(
        cars,
        Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada
}

void Velocity_draw(Car *player, float x, float y) {
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "%.1f", player->vel * 3.6f);
    DrawText(buffer, x, y, 64, WHITE);
}

void Game_draw() {
    int split = state.mode == SPLITSCREEN;

    if (split) {
        BeginScissorMode(0, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT);
    }

    BeginMode2D(*camera1);
    Map_draw();
    EndMode2D();

    Car *p1 = LinkedList_getCarById(cars, 1);
    Velocity_draw(p1, 128, SCREEN_HEIGHT - 2 * 64);

    if (split) {
        EndScissorMode();

        BeginScissorMode(SCREEN_WIDTH / 2.0f, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT);
        BeginMode2D(*camera2);
        Map_draw();
        EndMode2D();
        EndScissorMode();

        Car *p2 = LinkedList_getCarById(cars, 2);
        Velocity_draw(p2, SCREEN_WIDTH / 2.0f + 128, SCREEN_HEIGHT - 2 * 64);

        DrawRectangle(SCREEN_WIDTH / 2.0f - 5, 0, 10, SCREEN_HEIGHT, WHITE);
    }

    drawHud();

    if (IsKeyDown(KEY_Q)) {
        state.screen = MENU;
        mapCleanup();
    }
}

#include "arrayList.h"
#include "common.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// --- Variáveis internas ---

static ArrayList *bestLap    = NULL;
static ArrayList *currentLap = NULL;

static char ghostCarPath[100];

static int replayFrameIdx = 0;

// --- Funções internas ---

static void updateGhostCar(Car *player);
static void recordLap(Car *player);
static void showReplayBestLap();

static void updateBestLap();
static void loadBestLap();

static void drawGhostCarDebug();

//----------------------------------------------------------------------------------
// Carregando modo de jogo
//----------------------------------------------------------------------------------

void loadSingleplayer(Map map) {
    state.status = STARTED;
    minimapPos.x = SCREEN_WIDTH - trackHud.width;
    minimapPos.y = 10;

    strcpy(ghostCarPath, GHOST_CAR_DATA_PATH);
    strcat(ghostCarPath, map.name);
    strcat(ghostCarPath, ".bin");

    replayFrameIdx = 0;
    currentLap     = ArrayList_create();
    bestLap        = ArrayList_create();
    loadBestLap();

    if (ArrayList_length(bestLap) > 0) {
        bestLapTime = ArrayList_getLast(bestLap).time;
    }

    Car *ghostCar = Car_create((Vector2) {-1000, -1000}, 0, DEFAULT_CAR_CONFIG, CAR_IMAGES_PATH[0],
                               WHITE, true, 99, "Melhor Volta");
    Car *player   = Car_create(map.startCarPos[0], map.startAngle, DEFAULT_CAR_CONFIG,
                               CAR_IMAGES_PATH[0], WHITE, false, 1, "Player 1");

    LinkedList_addCar(cars, ghostCar);
    LinkedList_addCar(cars, player);

    Camera_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    camera1 = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
                            0.0f, 0.5f);
}

//----------------------------------------------------------------------------------
// Atualizando o jogo
//----------------------------------------------------------------------------------

void updateSingleplayer() {
    Car *p1 = LinkedList_getCarById(cars, 1);

    updateGhostCar(p1);
    Camera_updateTarget(camera1, p1);
    Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);
    Car_update(p1);
}

//----------------------------------------------------------------------------------
// Ghost Car
//----------------------------------------------------------------------------------

static void updateGhostCar(Car *player) {
    if (player->lap > 0 && player->changeLapFlag) {
        player->changeLapFlag = false;
        replayFrameIdx        = 0;

        if (ArrayList_length(currentLap) < ArrayList_length(bestLap) ||
            ArrayList_length(bestLap) == 0) {
            ArrayList_copy(bestLap, currentLap);
            updateBestLap();
            bestLapTimePlayer = player;
        }

        ArrayList_clear(currentLap);
    } else if (player->lap >= 0) {
        recordLap(player);
        showReplayBestLap();
    }
}

static void recordLap(Car *player) {
    CarFrame frameData = {player->pos, player->angle, GetTime() - player->startLapTime};
    ArrayList_push(currentLap, frameData);
}

static void showReplayBestLap() {
    Car *ghost = LinkedList_getCarById(cars, 99);
    if (replayFrameIdx < ArrayList_length(bestLap)) {
        CarFrame frameData = ArrayList_get(bestLap, replayFrameIdx++);
        ghost->pos         = frameData.pos;
        ghost->angle       = frameData.angle;
        ghost->ghostActive = true;
    }
}

static void loadBestLap() {
    FILE *file = fopen(ghostCarPath, "rb");

    if (!file) {
        file = fopen(ghostCarPath, "wb");
    }

    file = fopen(ghostCarPath, "rb");
    if (file != NULL) {
        CarFrame buffer;
        while (fread(&buffer, sizeof(CarFrame), 1, file) == 1) {
            ArrayList_push(bestLap, buffer);
        }
        fclose(file);
    }
}

static void updateBestLap() {
    FILE *file = fopen(ghostCarPath, "wb");
    if (file != NULL) {
        for (int i = 0; i < bestLap->length; i++) {
            fwrite(&bestLap->data[i], sizeof(CarFrame), 1, file);
        }
        fclose(file);
    }
}

//----------------------------------------------------------------------------------
// Draw
//----------------------------------------------------------------------------------

void drawSingleplayer() {
    BeginMode2D(*camera1);
    drawMap();
    EndMode2D();
}

void drawHudSingleplayer() {
    Car *p1 = LinkedList_getCarById(cars, 1);
    drawPlayerHud(p1, 0);

    if (state.debug) {
        drawGhostCarDebug();
    }
}

//----------------------------------------------------------------------------------
// Debug
//----------------------------------------------------------------------------------

static void drawGhostCarDebug() {
    sprintf(
        strBuffer,
        "Ghost car debug\nRecording i: %u\nPlayback i: %u\n\nCurrent lap debug\nRecording i: %u",
        ArrayList_length(bestLap), replayFrameIdx, ArrayList_length(currentLap));

    Vector2 size = MeasureTextEx(FONTS[0], strBuffer, 20, 1.0f);
    DrawRectangle(SCREEN_WIDTH - size.x - 10, 500, size.x + 10, size.y + 10,
                  (Color) {196, 196, 196, 200});
    DrawTextEx(FONTS[0], strBuffer, (Vector2) {SCREEN_WIDTH - size.x - 10, 500}, 20, 1.0f, BLACK);
}

//----------------------------------------------------------------------------------
// Limpeza final
//----------------------------------------------------------------------------------

void cleanUpSingleplayer() {
    ArrayList_free(bestLap);
    ArrayList_free(currentLap);
}

#include "common.h"
#include "game.h"
#include "menu.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// --- Variáveis internas ---

static ArrayList *bestLap    = NULL;
static ArrayList *currentLap = NULL;

static char ghostCarPath[100];

static int lastLap        = 0;
static int replayFrameIdx = 0;

static bool   flagBestLap = 0;
static double msgStart;
static int    msgActive;
static int    msgCount;

// --- Funções internas ---

static void updateGhostCar(Car *player);
static void updateBestLap();
static void loadBestLap();

static void drawBestLapTime(Car *player, float x, float y);
static void drawBestLapMessage(float x, float y, int size, Color color, char *text);
static void drawGhostCarDebug();

//----------------------------------------------------------------------------------
// Carregando modo de jogo
//----------------------------------------------------------------------------------

void loadSingleplayer(Map map) {
    flagBestLap  = false;
    msgStart     = 0;
    msgActive    = 0;
    msgCount     = 0;
    state.status = STARTED;
    minimapPos.x = SCREEN_WIDTH - trackHud.width;
    minimapPos.y = 10;

    strcpy(ghostCarPath, GHOST_CAR_DATA_PATH);
    strcat(ghostCarPath, map.name);
    strcat(ghostCarPath, ".bin");

    replayFrameIdx = 0;
    lastLap        = 0;
    currentLap     = ArrayList_create();
    bestLap        = ArrayList_create();
    loadBestLap();

    Car *ghostCar = Car_create((Vector2) {-1000, -1000}, 0, DEFAULT_CAR_CONFIG, CAR_IMAGES_PATH[0],
                               WHITE, true, 99);
    Car *player   = Car_create(map.startCarPos[0], map.startAngle, DEFAULT_CAR_CONFIG,
                               CAR_IMAGES_PATH[0], WHITE, false, 1);

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
    Car *ghost = LinkedList_getCarById(cars, 99);
    if (player->lap > lastLap) {
        lastLap        = player->lap;
        replayFrameIdx = 0;
        ArrayList_push(currentLap, (GhostCarFrame) {(Vector2) {-1000, -1000}, 0});
        if (ArrayList_length(currentLap) < ArrayList_length(bestLap) ||
            ArrayList_length(bestLap) == 0) {
            ArrayList_copy(bestLap, currentLap);
            updateBestLap();
            flagBestLap = 1;
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

static void loadBestLap() {
    FILE *file = fopen(ghostCarPath, "rb");

    if (!file) {
        file = fopen(ghostCarPath, "wb");
    }

    file = fopen(ghostCarPath, "rb");
    if (file != NULL) {
        GhostCarFrame buffer;
        while (fread(&buffer, sizeof(GhostCarFrame), 1, file) == 1) {
            ArrayList_push(bestLap, buffer);
        }
        fclose(file);
    }
}

static void updateBestLap() {
    FILE *file = fopen(ghostCarPath, "wb");
    if (file != NULL) {
        for (int i = 0; i < bestLap->length; i++) {
            fwrite(&bestLap->data[i], sizeof(GhostCarFrame), 1, file);
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
    drawBestLapTime(p1, 32, 144);

    if (flagBestLap) {
        snprintf(textBuffer, sizeof(textBuffer), "Melhor Volta");
        drawBestLapMessage(
            SCREEN_WIDTH / 2.0f - MeasureTextEx(FONTS[1], textBuffer, 64, 1.0f).x / 2.0f,
            SCREEN_HEIGHT * 0.5f / 4.0f, 64, (Color) {158, 24, 181, 255}, textBuffer);
    }

    if (state.debug) {
        Car_showInfo(p1, 20, 300, 20, BLACK);
        drawGhostCarDebug();
    }
}

//----------------------------------------------------------------------------------
// Funções complementares para o draw
//----------------------------------------------------------------------------------

static void drawBestLapTime(Car *player, float x, float y) {
    if (player->lap > -1 && ArrayList_length(bestLap) > 0) {
        double time = (ArrayList_length(bestLap) - 1) / 60.0f;
        int    mins = time / 60;
        float  secs = time - (mins * 60);
        snprintf(textBuffer, sizeof(textBuffer), "%d:%05.2fs", mins, secs);
        drawTextWithShadow(textBuffer, x, y, 42, (Color) {158, 24, 181, 255}, FONTS[0]);
    }
}

static void drawBestLapMessage(float x, float y, int size, Color color, char *text) {
    float actualTime = GetTime();
    if (actualTime - msgStart >= 0.3f) {
        msgActive = !msgActive;
        msgStart  = actualTime;

        if (msgActive) {
            msgCount++;
        }

        if (msgCount > 3) {
            msgActive   = 0;
            msgCount    = 0;
            flagBestLap = false;
            return;
        }
    }

    if (msgActive) {
        drawTextWithShadow(text, x, y, size, color, FONTS[1]);
    }
}

static void drawGhostCarDebug() {
    sprintf(textBuffer, "Ghost car debug:\nRecording i: %u\nPlayback i: %u",
            ArrayList_length(bestLap), replayFrameIdx);
    DrawText(textBuffer, SCREEN_WIDTH - 200, 500, 20, BLACK);

    sprintf(textBuffer, "Current lap debug:\nRecording i: %d", ArrayList_length(currentLap));
    DrawText(textBuffer, SCREEN_WIDTH - 200, 600, 20, BLACK);
}

//----------------------------------------------------------------------------------
// Limpeza final
//----------------------------------------------------------------------------------

void cleanUpSingleplayer() {
    ArrayList_free(bestLap);
    ArrayList_free(currentLap);
}

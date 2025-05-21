#include "common.h"
#include "game.h"
#include "menu.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// --- Variáveis públicas ---

Car *winner   = NULL;
int  MAX_LAPS = 3;

// --- Variáveis internas ---

static Sound  semaphoreSound;
static double lastSoundTime;
static int    count;

// --- Funções internas ---

static void updateSemaphore();
static void updateWinner(Car *player);

static void drawSemaphore(float x, float y, int size);
static void drawView(Camera2D *camera, Rectangle scissor);

//----------------------------------------------------------------------------------
// Carregando modo de jogo
//----------------------------------------------------------------------------------

void loadSplitscreen(Map map) {
    lastSoundTime = GetTime() - 0.5;
    count         = 0;
    state.status  = COUNTDOWN;
    winner        = NULL;
    minimapPos.x  = SCREEN_WIDTH - trackHud.width;
    minimapPos.y  = 10;

    semaphoreSound = LoadSound(SMAPHORE_SOUND_PATH);

    Car *p1 = Car_create(map.startCarPos[0], map.startAngle, DEFAULT_CAR_CONFIG, CAR_IMAGES_PATH[1],
                         BLUE, false, 1);

    Car *p2 = Car_create(map.startCarPos[1], map.startAngle, DEFAULT_CAR_CONFIG, CAR_IMAGES_PATH[2],
                         ORANGE, false, 2);

    LinkedList_addCar(cars, p1);
    LinkedList_addCar(cars, p2);

    Camera_setSize(SCREEN_WIDTH / 2, SCREEN_HEIGHT);

    camera1 =
        Camera_create(p1->pos, (Vector2) {SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f}, 0.0f, 0.5f);
    camera2 = Camera_create(p2->pos, (Vector2) {SCREEN_WIDTH * 3.0f / 4.0f, SCREEN_HEIGHT / 2.0f},
                            0.0f, 0.5f);
}

//----------------------------------------------------------------------------------
// Atualizando o jogo
//----------------------------------------------------------------------------------

void updateSplitscreen() {
    if (state.status == COUNTDOWN) {
        updateSemaphore();
        return;
    }

    Car *p1 = LinkedList_getCarById(cars, 1);
    Camera_updateTarget(camera1, p1);
    Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);

    Car *p2 = LinkedList_getCarById(cars, 2);
    Camera_updateTarget(camera2, p2);
    Car_move(p2, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT);

    LinkedList_forEach(cars, updateWinner);
    LinkedList_forEach(cars, Car_update);
}

//----------------------------------------------------------------------------------
// Funções complementares para o update
//----------------------------------------------------------------------------------

static void updateSemaphore() {
    if (count == 4) {
        SetSoundPitch(semaphoreSound, 1.2);
        PlaySound(semaphoreSound);
        state.status = STARTED;
    }

    float actualTime = GetTime();
    if (actualTime - lastSoundTime > 1.0) {
        SetSoundPitch(semaphoreSound, 0.8);
        PlaySound(semaphoreSound);
        lastSoundTime = actualTime;
        count++;
    }
}

static void updateWinner(Car *player) {
    if (player->lap == MAX_LAPS) {
        winner       = player;
        state.status = ENDED;
    }
}

//----------------------------------------------------------------------------------
// Draw
//----------------------------------------------------------------------------------

void drawSplitscreen() {

    drawView(camera1, (Rectangle) {0, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT});
    drawView(camera2, (Rectangle) {SCREEN_WIDTH / 2.0f, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT});

    DrawRectangle(SCREEN_WIDTH / 2.0f - 5, 0, 10, SCREEN_HEIGHT, (Color) {51, 51, 51, 255});

    if (winner) {
        snprintf(textBuffer, sizeof(textBuffer), "Jogador %d Ganhou", winner->id);
        drawTextWithShadow(textBuffer,
                           (SCREEN_WIDTH - MeasureTextEx(FONTS[1], textBuffer, 128, 1.0f).x) / 2.0f,
                           (SCREEN_HEIGHT - 128) * 1.0f / 4.0f, 128, YELLOW, FONTS[1]);
        return;
    }

    if (state.status == COUNTDOWN) {
        drawSemaphore(SCREEN_WIDTH * 1.0f / 4.0f, SCREEN_HEIGHT / 2.0, 48);
        drawSemaphore(SCREEN_WIDTH * 3.0f / 4.0f, SCREEN_HEIGHT / 2.0, 48);
        return;
    }
}

void drawHudSplitscreen() {
    Car *p1 = LinkedList_getCarById(cars, 1);
    drawPlayerHud(p1, 0);

    Car *p2 = LinkedList_getCarById(cars, 2);
    drawPlayerHud(p2, SCREEN_WIDTH / 2);
}

//----------------------------------------------------------------------------------
// Funções complementares para o draw
//----------------------------------------------------------------------------------

static void drawView(Camera2D *camera, Rectangle scissor) {
    BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
    BeginMode2D(*camera);
    drawMap();
    EndMode2D();
    EndScissorMode();
}

static void drawSemaphore(float x, float y, int size) {
    DrawCircle(x - 3 * size, y, size, count >= 1 ? RED : BLACK);
    DrawCircle(x, y, size, count >= 2 ? RED : BLACK);
    DrawCircle(x + 3 * size, y, size, count >= 3 ? RED : BLACK);
}

//----------------------------------------------------------------------------------
// Limpeza final
//----------------------------------------------------------------------------------

void cleanUpSplitscreen() {
    Camera_free(camera2);
    UnloadSound(semaphoreSound);
}

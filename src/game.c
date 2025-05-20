#include "game.h"
#include "arrayList.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"
#include "raylib.h"
#include "stdio.h"
#include <stdio.h>
#include <string.h>

static Texture2D   trackBackground;
static Texture2D   trackHud;
static LinkedList *cars;

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

static Vector2 minimapPos;

int    winner;
int    maxLaps;
int    countdown;
double lastDecrement;

// --- Funções públicas ---
void Game_setup();
void Game_load();
void Game_update();
void Game_draw();
void Game_cleanup();

// --- Funções internas ---
static void loadMap(Map map);
static void mapCleanup();

static void loadSingleplayer(Map map);
static void loadSplitscreen(Map map);

static void loadBestLap();
static void updateBestLap();
static void updateGhostCar(Car *player);

static void drawTextWithShadow(char *text, float x, float y, int size, Color color);
static void drawDebugInfo(Car *player, Car *ghost);
static void drawPlayerInMinimap(Car *player);
static void drawSpeedometer(Car *player, float x, float y);
static void drawLaps(Car *player, float x, float y);
static void drawHud();
static void drawMap();

//----------------------------------------------------------------------------------
// Carregamento do jogo
//----------------------------------------------------------------------------------

void Game_setup() {
    cars = LinkedList_create();
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

void Game_cleanup() {
    if (state.screen != GAME) {
        mapCleanup();
    }
    LinkedList_free(cars);
}

//----------------------------------------------------------------------------------
// Atualização dos componentes do jogo (carros, camera...)
//----------------------------------------------------------------------------------

void Game_update() {
    if (IsKeyDown(KEY_Q)) {
        state.screen = MENU;
        mapCleanup();
        return;
    }

    Car *p1 = LinkedList_getCarById(cars, 1);

    if (state.mode == SINGLEPLAYER) {
        SetMusicPitch(carSound, 0.6 + p1->vel / 13.0f);
        SetMusicVolume(music, GAME_MUSIC_VOLUME);
        UpdateMusicStream(music);
        UpdateMusicStream(carSound);

        updateGhostCar(p1);

        Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);

        LinkedList_forEach(cars, Car_update);
    } else {
        if (countdown > 0) {
            if (GetTime() - lastDecrement >= 1 || lastDecrement == -1) {
                countdown--;
                lastDecrement = GetTime();
            }
            return;
        } else if (winner > 0) {
            if (countdown == 0) {
                state.screen = MENU;
                mapCleanup();
                return;
            }

            if (GetTime() - lastDecrement >= 1 || lastDecrement == -1) {
                countdown--;
                lastDecrement = GetTime();
            }
            return;
        }

        Car *p2 = LinkedList_getCarById(cars, 2);

        Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);
        Car_move(p2, KEY_I, KEY_K, KEY_L, KEY_J);

        LinkedList_forEach(cars, Car_update);

        if (p1->lap + 1 > maxLaps || p2->lap + 1 > maxLaps) {
            winner    = p1->lap + 1 > maxLaps ? p1->id : p2->id;
            countdown = 3 + 1;
            return;
        }

        Camera_updateTarget(camera2, p2);
    }

    Camera_updateTarget(camera1, p1);
}

//----------------------------------------------------------------------------------
// Desenhando a tela do jogo
//----------------------------------------------------------------------------------

void Game_draw() {
    if (state.screen != GAME) {
        return;
    }

    int split = state.mode == SPLITSCREEN;

    if (split) {
        BeginScissorMode(0, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT);
    }

    BeginMode2D(*camera1);
    drawMap();
    EndMode2D();

    Car *p1 = LinkedList_getCarById(cars, 1);
    drawSpeedometer(p1, 128, SCREEN_HEIGHT - 2 * 64);
    drawLaps(p1, 32, 32);

    if (split) {
        EndScissorMode();

        BeginScissorMode(SCREEN_WIDTH / 2.0f, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT);
        BeginMode2D(*camera2);
        drawMap();
        EndMode2D();
        EndScissorMode();

        Car *p2 = LinkedList_getCarById(cars, 2);
        drawSpeedometer(p2, SCREEN_WIDTH / 2.0f + 128, SCREEN_HEIGHT - 2 * 64);
        drawLaps(p2, SCREEN_WIDTH / 2.0f + 32, 32);

        DrawRectangle(SCREEN_WIDTH / 2.0f - 5, 0, 10, SCREEN_HEIGHT, (Color) {51, 51, 51, 255});
    }

    drawHud();

    if (split && countdown) {
        char buffer[32];
        if (winner > 0) {
            int font = 128;
            snprintf(buffer, sizeof(buffer), "Jogador %d Ganhou", winner);
            drawTextWithShadow(buffer, (SCREEN_WIDTH / 2.0f) - (MeasureText(buffer, font) / 2.0f),
                               SCREEN_HEIGHT / 2.0f - font, font, YELLOW);
        } else {
            snprintf(buffer, sizeof(buffer), "%d", countdown);
            drawTextWithShadow(buffer, SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f - 128, 256, WHITE);
            drawTextWithShadow(buffer, SCREEN_WIDTH * 3.0f / 4.0f, SCREEN_HEIGHT / 2.0f - 128, 256,
                               WHITE);
        }
    }
}

//----------------------------------------------------------------------------------
// Carregamento do mapa
//----------------------------------------------------------------------------------

static void loadMap(Map map) {
    trackBackground = LoadTexture(state.debug ? map.maskPath : map.backgroundPath);

    Image minimap = LoadImage(state.debug ? map.maskPath : map.minimapPath);
    minimapWidth  = SCREEN_WIDTH / 4;
    minimapHeigth = SCREEN_HEIGHT / 4;
    ImageResize(&minimap, minimapWidth, minimapHeigth);
    trackHud = LoadTextureFromImage(minimap);
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

static void mapCleanup() {
    Track_Unload();
    LinkedList_clear(cars);
    UnloadTexture(trackBackground);
    UnloadTexture(trackHud);
    if (state.mode == SINGLEPLAYER) {
        ArrayList_free(bestLap);
        ArrayList_free(currentLap);
    } else {
        Camera_free(camera2);
    }
    Camera_free(camera1);
    UnloadMusicStream(music);
    UnloadMusicStream(carSound);
}

//----------------------------------------------------------------------------------
// Carregando modo de jogo
//----------------------------------------------------------------------------------

static void loadSingleplayer(Map map) {
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

static void loadSplitscreen(Map map) {
    winner        = -1;
    maxLaps       = 3;
    countdown     = 3 + 1; // 3 + 1 segundo
    lastDecrement = -1;

    minimapPos.x = SCREEN_WIDTH - trackHud.width;
    minimapPos.y = 10;

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
// Ghost Car (singleplayer)
//----------------------------------------------------------------------------------

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
            updateBestLap();
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

//----------------------------------------------------------------------------------
// Draw
//----------------------------------------------------------------------------------

static void drawTextWithShadow(char *text, float x, float y, int size, Color color) {
    DrawText(text, x + 1, y + 1, size, BLACK);
    DrawText(text, x, y, size, color);
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

static void drawPlayerInMinimap(Car *player) {
    float x = trackHud.width * player->pos.x / trackBackground.width + minimapPos.x;
    float y = trackHud.height * player->pos.y / trackBackground.height + minimapPos.y;

    if (player->ghost) {
        DrawCircleLines(x, y, 3.5f, BLACK);
        DrawCircle(x, y, 3, WHITE);
    } else {
        DrawCircleLines(x, y, 6.5f, BLACK);
        DrawCircle(x, y, 6, player->color);
    }
}

static void drawHud() {
    if (state.debug) {
        Car *player = LinkedList_getCarById(cars, 1);

        Car *ghost = LinkedList_getCarById(cars, 99);
        drawDebugInfo(player, ghost);
    }

    DrawTexture(trackHud, minimapPos.x, minimapPos.y, (Color) {255, 255, 255, HUD_OPACITY});
    LinkedList_forEach(cars, drawPlayerInMinimap);
}

void drawMap() {
    DrawTexture(trackBackground, 0, 0, WHITE);
    LinkedList_forEach(cars, Car_draw);
}

void drawSpeedometer(Car *player, float x, float y) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.1f km/h",
             3600 * 1.1 * player->vel * 60 / trackBackground.width / 1.5f);
    Color textColor = ColorLerp(WHITE, RED, player->vel / player->maxVelocity);
    drawTextWithShadow(buffer, x, y, 64, textColor);
}

void drawLaps(Car *player, float x, float y) {
    char buffer[32];

    if (player->lap > -1) {
        if (state.mode == SPLITSCREEN) {
            snprintf(buffer, sizeof(buffer), "Volta %d/%d", player->lap + 1, maxLaps);
        } else {
            snprintf(buffer, sizeof(buffer), "Volta %d", player->lap + 1);
        }
    }

    drawTextWithShadow(buffer, x, y, 64, WHITE);
}

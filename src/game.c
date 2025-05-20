#include "game.h"
#include "arrayList.h"
#include "camera.h"
#include "common.h"
#include "linked_list.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

static Texture2D   trackBackground;
static Texture2D   trackHud;
static LinkedList *cars;

static Camera2D *camera1;
static Camera2D *camera2;

static char ghostCarPath[100];
static char textBuffer[1000];

static int minimapWidth;
static int minimapHeigth;

static ArrayList *bestLap    = NULL;
static ArrayList *currentLap = NULL;

static int lastLap        = 0;
static int replayFrameIdx = 0;

static Music  music;
static Sound  semaphoreSound;
static double lastSoundTime;
static int    count;

static Vector2 minimapPos;

static Car *winner = NULL;

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

static void updateWinner(Car *player);

static void drawMap();
void        drawView(Camera2D *camera, Rectangle scissor);

static void drawHud();

static void drawSemaphore(float x, float y, int size);
static void drawTextWithShadow(char *text, float x, float y, int size, Color color);
static void drawBestLapTime(Car *player, float x, float y);
static void drawLapTime(Car *player, float x, float y);
static void drawGhostCarDebug();
static void drawPlayerInMinimap(Car *player);
static void drawSpeedometer(Car *player, float x, float y);
static void drawLaps(Car *player, float x, float y);

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

    UpdateMusicStream(music);

    Car *p1 = LinkedList_getCarById(cars, 1);

    if (state.mode == SINGLEPLAYER) {
        updateGhostCar(p1);
    } else {
        if (state.status == COUNTDOWN) {
            if (GetTime() - state.raceTime > 3.5f) {
                SetSoundPitch(semaphoreSound, 1.2);
                PlaySound(semaphoreSound);
                state.status = STARTED;
            }
            return;
        }

        Car *p2 = LinkedList_getCarById(cars, 2);

        if (winner == NULL) {
            LinkedList_forEach(cars, updateWinner);
        }

        if (winner) {
            if (GetTime() - winner->startLapTime > 3) {
                state.screen = MENU;
                mapCleanup();
            }
            return;
        }

        Car_move(p2, KEY_I, KEY_K, KEY_L, KEY_J);

        Camera_updateTarget(camera2, p2);
    }
    Car_move(p1, KEY_W, KEY_S, KEY_D, KEY_A);
    Camera_updateTarget(camera1, p1);
    LinkedList_forEach(cars, Car_update);
}

//----------------------------------------------------------------------------------
// Desenhando a tela do jogo
//----------------------------------------------------------------------------------

void Game_draw() {
    if (state.screen != GAME)
        return;

    if (state.mode == SPLITSCREEN) {
        drawView(camera1, (Rectangle) {0, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT});
        drawView(camera2, (Rectangle) {SCREEN_WIDTH / 2.0f, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT});

        DrawRectangle(SCREEN_WIDTH / 2.0f - 5, 0, 10, SCREEN_HEIGHT, (Color) {51, 51, 51, 255});

        if (state.status == COUNTDOWN) {
            drawSemaphore(SCREEN_WIDTH * 1.0f / 4.0f, SCREEN_HEIGHT / 2.0, 48);
            drawSemaphore(SCREEN_WIDTH * 3.0f / 4.0f, SCREEN_HEIGHT / 2.0, 48);
            return;
        }

        if (winner) {
            snprintf(textBuffer, sizeof(textBuffer), "Jogador %d Ganhou", winner->id);
            int textWidth = MeasureText(textBuffer, WINNER_FONT_SIZE);
            drawTextWithShadow(textBuffer, (SCREEN_WIDTH - textWidth) / 2.0f,
                               (SCREEN_HEIGHT - WINNER_FONT_SIZE) / 2.0f, WINNER_FONT_SIZE, YELLOW);
        }
    } else {
        // Tela única
        BeginMode2D(*camera1);
        drawMap();
        EndMode2D();
    }

    if (state.status == STARTED) {
        drawHud();
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

    music = LoadMusicStream(GAME_MUSIC_PATH);
    SetMusicVolume(music, GAME_MUSIC_VOLUME);
    PlayMusicStream(music);
}

static void mapCleanup() {
    Track_Unload();
    LinkedList_clear(cars);
    UnloadTexture(trackBackground);
    UnloadTexture(trackHud);
    if (state.mode == SINGLEPLAYER) {
        ArrayList_free(bestLap);
        ArrayList_free(currentLap);
        UnloadSound(semaphoreSound);
    } else {
        Camera_free(camera2);
    }
    Camera_free(camera1);
    UnloadMusicStream(music);
}

//----------------------------------------------------------------------------------
// Carregando modo de jogo
//----------------------------------------------------------------------------------

static void loadSingleplayer(Map map) {
    state.status   = STARTED;
    state.raceTime = GetTime();
    minimapPos.x   = SCREEN_WIDTH - trackHud.width;
    minimapPos.y   = 10;

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
    lastSoundTime  = 0;
    count          = 0;
    state.status   = COUNTDOWN;
    state.raceTime = GetTime();
    winner         = NULL;
    minimapPos.x   = SCREEN_WIDTH - trackHud.width;
    minimapPos.y   = 10;

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
// Atualizando o vencedor
//----------------------------------------------------------------------------------

static void updateWinner(Car *player) {
    if (player->lap == MAX_LAPS) {
        winner       = player;
        state.status = ENDED;
        return;
    }
}

//----------------------------------------------------------------------------------
// Draw map
//----------------------------------------------------------------------------------

static void drawMap() {
    DrawTexture(trackBackground, 0, 0, WHITE);
    LinkedList_forEach(cars, Car_draw);
}

void drawView(Camera2D *camera, Rectangle scissor) {
    BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
    BeginMode2D(*camera);
    drawMap();
    EndMode2D();
    EndScissorMode();
}

//----------------------------------------------------------------------------------
// Draw hud
//----------------------------------------------------------------------------------

static void drawHud() {
    if (winner)
        return;
    Car *p1 = LinkedList_getCarById(cars, 1);
    drawSpeedometer(p1, 128, SCREEN_HEIGHT - 2 * 64);
    drawLaps(p1, 32, 32);
    drawLapTime(p1, 32, 96);
    if (state.mode == SINGLEPLAYER) {
        drawBestLapTime(p1, 32, 144);
    }
    if (state.debug) {
        Car_showInfo(p1, 20, 300, 20, BLACK);
        if (state.mode == SINGLEPLAYER)
            drawGhostCarDebug();
    }

    if (state.mode == SPLITSCREEN) {
        Car *p2 = LinkedList_getCarById(cars, 2);
        drawSpeedometer(p2, SCREEN_WIDTH / 2.0f + 128, SCREEN_HEIGHT - 2 * 64);
        drawLaps(p2, SCREEN_WIDTH / 2.0f + 32, 32);
        drawLapTime(p2, SCREEN_WIDTH / 2.0f + 32, 96);
        if (state.debug) {
            Car_showInfo(p2, SCREEN_WIDTH - 400, 300, 20, BLACK);
        }
    }

    DrawTexture(trackHud, minimapPos.x, minimapPos.y, (Color) {255, 255, 255, HUD_OPACITY});
    LinkedList_forEach(cars, drawPlayerInMinimap);
}

//----------------------------------------------------------------------------------
// Funções auxiliares para desenhar a hud
//----------------------------------------------------------------------------------

static void drawTextWithShadow(char *text, float x, float y, int size, Color color) {
    DrawText(text, x + 1, y + 1, size, BLACK);
    DrawText(text, x, y, size, color);
}

static void drawGhostCarDebug() {
    sprintf(textBuffer, "Ghost car debug:\nRecording i: %u\nPlayback i: %u",
            ArrayList_length(bestLap), replayFrameIdx);
    DrawText(textBuffer, SCREEN_WIDTH - 200, 500, 20, BLACK);

    sprintf(textBuffer, "Current lap debug:\nRecording i: %d", ArrayList_length(currentLap));
    DrawText(textBuffer, SCREEN_WIDTH - 200, 600, 20, BLACK);
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

static void drawSpeedometer(Car *player, float x, float y) {
    snprintf(textBuffer, sizeof(textBuffer), "%.1f km/h",
             3600 * 1.1 * player->vel * 60 / trackBackground.width / 1.5f);
    Color textColor = ColorLerp(WHITE, RED, player->vel / player->maxVelocity);
    drawTextWithShadow(textBuffer, x, y, 64, textColor);
}

static void drawLaps(Car *player, float x, float y) {
    if (player->lap > -1) {
        if (state.mode == SINGLEPLAYER) {
            snprintf(textBuffer, sizeof(textBuffer), "Volta %d", player->lap + 1);
        } else if (player->lap < MAX_LAPS) {
            snprintf(textBuffer, sizeof(textBuffer), "Volta %d/%d", player->lap + 1, MAX_LAPS);
        }
        drawTextWithShadow(textBuffer, x, y, 64, WHITE);
    }
}

static void drawLapTime(Car *player, float x, float y) {
    if (player->lap > -1) {
        double time = GetTime() - player->startLapTime;
        int    mins = time / 60;
        float  secs = time - (mins * 60);
        snprintf(textBuffer, sizeof(textBuffer), "%d:%05.2fs", mins, secs);
        drawTextWithShadow(textBuffer, x, y, 48, WHITE);
    }
}

static void drawBestLapTime(Car *player, float x, float y) {
    if (player->lap > -1 && ArrayList_length(bestLap) > 0) {
        double time = (ArrayList_length(bestLap) - 1) / 60.0f;
        int    mins = time / 60;
        float  secs = time - (mins * 60);
        snprintf(textBuffer, sizeof(textBuffer), "%d:%05.2fs", mins, secs);
        drawTextWithShadow(textBuffer, x, y, 42, (Color) {158, 24, 181, 255});
    }
}

static void drawSemaphore(float x, float y, int size) {
    if (GetTime() - state.raceTime >= 0.5f && GetTime() - lastSoundTime >= 1.0) {
        SetSoundPitch(semaphoreSound, 0.8);
        PlaySound(semaphoreSound);
        lastSoundTime = GetTime();
        count++;
    }

    DrawCircle(x - 3 * size, y, size, count >= 1 ? RED : BLACK);
    DrawCircle(x, y, size, count >= 2 ? RED : BLACK);
    DrawCircle(x + 3 * size, y, size, count >= 3 ? RED : BLACK);
}

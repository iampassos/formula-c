#include "common.h"
#include "game.h"
#include "menu.h"

int SCREEN_WIDTH  = 0;
int SCREEN_HEIGHT = 0;

char *gameName       = "FORMULA C";
char *backgroundPath = "resources/menu/f12024.png";

float carVolume       = 0.2f;
float gameMusicVolume = 1.0f;

char *gameMusicPath = "resources/sounds/game-music.mp3";
char *menuMusicPath = "resources/sounds/menu-music.mp3";
char *carSoundPath  = "resources/sounds/f1s.mp3";

char *clickButtonSoundPath = "resources/sounds/click.mp3";

Map maps[] = {{"resources/masks/interlagos_maskV2.png", // backgroundPath
               "resources/masks/interlagos_maskV2.png", // maskPath
               {{/*Color*/ {0, 255, 0}, /*Vector2*/ {4371, 2537}, 2.66f},
                {/*Color*/ {0, 0, 255}, /*Vector2*/ {6700, 8147}, 0.0f},
                {/*Color*/ {255, 0, 0}, /*Vector2*/ {11069, 2257}, 2.17f}}}};

// Areas da pista
TrackArea TRACK_AREAS[] = {/* pista */ {{127, 127, 127}, 0.99},
                           /* fraca */ {{255, 127, 39}, 0.96},
                           /* forte */ {{163, 73, 164}, 0.93},
                           /* grama */ {{34, 177, 76}, 0.98}};

int TRACK_AREA_SIZE;

// Cor que representa fora da pista
Color OUTSIDE_TRACK_COLOR = {255, 255, 255};

State state = {0};

int main() {
    TRACK_AREA_SIZE = sizeof(TRACK_AREAS) / sizeof(TrackArea);
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Formula C");

    SCREEN_WIDTH  = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    Image icon = LoadImage("resources/logo/formula_c-logo.png");
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);
    InitAudioDevice();

    SetTargetFPS(60);

    setup_menu();

    while (!WindowShouldClose()) {
        switch (state.screen) {
        case MENU:
            update_menu();
            BeginDrawing();
            draw_menu();
            EndDrawing();
            break;
        case GAME:
            update_game();
            BeginDrawing();
            draw_game();
            EndDrawing();
            break;
        }
    }

    cleanup_game();
    cleanup_menu();
    CloseAudioDevice();
    CloseWindow(); // Fechar a janela gráfica 2d

    return 0;
}

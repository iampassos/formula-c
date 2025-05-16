#include "common.h"
#include "game.h"
#include "menu.h"

// Configurações de tela
int SCREEN_WIDTH  = 0;
int SCREEN_HEIGHT = 0;

// Informações do jogo
char *GAME_NAME         = "FORMULA C";
char  GAME_MODES[][100] = {"1 Jogador", "2 Jogadores"};
int   TOTAL_GAME_MODES  = sizeof(GAME_MODES) / sizeof(GAME_MODES[0]);

// Dados do ghost car
char *BEST_LAP_DATA_PATH = "./data/best_lap.bin";

// Áudio
char *GAME_MUSIC_PATH         = "resources/sounds/game-music.mp3";
char *MENU_MUSIC_PATH         = "resources/sounds/menu-music.mp3";
char *CAR_SOUND_PATH          = "resources/sounds/f1s.mp3";
char *CLICK_BUTTON_SOUND_PATH = "resources/sounds/click.mp3";

// float CAR_VOLUME        = 0.2f;
// float GAME_MUSIC_VOLUME = 1.0f;
// float MENU_MUSIC_VOLUME = 1.0f;
float CAR_VOLUME        = 0.0f;
float GAME_MUSIC_VOLUME = 0.0f;
float MENU_MUSIC_VOLUME = 0.0f;

// Recursos visuais
char *BACKGROUND_PATH = "resources/menu/f12024.png";
char *CAR_IMAGE_PATH  = "resources/cars/carroazul.png";
char *LOGO_IMAGE_PATH = "resources/logo/formula_c-logo.png";
int   HUD_OPACITY     = 200;

// Mapas
Map MAPS[] = {{"Interlagos",
               "resources/masks/interlagos_maskV2.png", // backgroundPath
               "resources/masks/interlagos_maskV2.png", // maskPath
               {5400, 2000},
               2.66,
               3,
               {{/*Vector2*/ {4371, 2537}, 2.66f, true},
                {/*Vector2*/ {6700, 8147}, 0.0f, false},
                {/*Vector2*/ {8000, 4143}, -2.63, false},
                {/*Vector2*/ {11069, 2257}, 2.17f, false}}},

              {"Debug Map",
               "resources/masks/pista_debug_mask.png",
               "resources/masks/pista_debug_mask.png",
               {6900, 2657},
               0,
               3,
               {{/*Vector2*/ {4371, 2537}, 2.66f, true},
                {/*Vector2*/ {6700, 8147}, 0.0f, false},
                {/*Vector2*/ {11069, 2257}, 2.17f, false}}}};

int TOTAL_MAPS = sizeof(MAPS) / sizeof(Map);

// Áreas da pista
TrackArea TRACK_AREAS[] = {
    /* pista  */ {{127, 127, 127}, 0.99},
    /* fraca  */ {{255, 127, 39}, 0.96},
    /* forte  */ {{163, 73, 164}, 0.93},
    /* grama  */ {{34, 177, 76}, 0.98}};

int TRACK_AREA_SIZE = sizeof(TRACK_AREAS) / sizeof(TrackArea);

// Pista selecionada
int SELECTED_MAP_IDX = 0;

// Cores da pista
Color OUTSIDE_TRACK_COLOR = {255, 255, 255};
Color RACE_START_COLOR    = {0, 0, 255};
Color CHECKPOINT_COLOR    = {0, 255, 0};

// Estado do jogo
State state = {0};

int main() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Formula C");

    SCREEN_WIDTH  = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    Image icon = LoadImage(LOGO_IMAGE_PATH);
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);
    InitAudioDevice();

    SetTargetFPS(60);

    Menu_setup();
    Game_setup();

    while (!WindowShouldClose()) {
        switch (state.screen) {
        case MENU:
            Menu_update();
            BeginDrawing();
            Menu_draw();
            EndDrawing();
            break;
        case GAME:
            Game_update();
            BeginDrawing();
            Game_draw();
            EndDrawing();
            break;
        }
    }

    Game_cleanup();
    Menu_cleanup();
    CloseAudioDevice();
    CloseWindow(); // Fechar a janela gráfica 2d

    return 0;
}

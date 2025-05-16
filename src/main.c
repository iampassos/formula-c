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

// Pasta de gravação dos arquivos dos carros fantasmas
char *GHOST_CAR_DATA_PATH = "./data/";

// Áudio
char *GAME_MUSIC_PATH         = "resources/sounds/game-music.mp3";
char *MENU_MUSIC_PATH         = "resources/sounds/menu-music.mp3";
char *CAR_SOUND_PATH          = "resources/sounds/f1s.mp3";
char *CLICK_BUTTON_SOUND_PATH = "resources/sounds/click.mp3";
float CAR_VOLUME              = 0.0f;
float GAME_MUSIC_VOLUME       = 0.0f;
float MENU_MUSIC_VOLUME       = 0.0f;

// Recursos visuais
char *BACKGROUND_PATH = "resources/menu/f12024.png";
char *CAR_IMAGE_PATH  = "resources/cars/carroazul.png";
char *LOGO_IMAGE_PATH = "resources/logo/formula_c-logo.png";
int   HUD_OPACITY     = 200;

// Mapas
Map MAPS[] = {{"Interlagos",
               "resources/maps/interlagos_map.png",   // backgroundPath
               "resources/masks/interlagos_mask.png", // maskPath
               "resources/minimaps/interlagos_minimap.png",
               {5005, 1884},
               2.66,
               10,
               {
                   {/*Vector2*/ {4645, 2050}, 2.73f},
                   {/*Vector2*/ {1816, 6000}, 1.75f},
                   {/*Vector2*/ {3796, 8174}, 0.0f},
                   {/*Vector2*/ {10322, 8174}, 0.0f},
                   {/*Vector2*/ {9602, 5208}, -2.77f},
                   {/*Vector2*/ {7272, 2106}, -0.63f},
                   {/*Vector2*/ {9721, 2638}, -1.09f},
                   {/*Vector2*/ {12191, 4876}, 0.07f},
                   {/*Vector2*/ {13993, 2450}, -1.87f},
                   {/*Vector2*/ {8320, 487}, -3.57f},
               }},

              {"Debug Map",
               "resources/masks/pista_debug_mask.png",
               "resources/masks/pista_debug_mask.png",
               "resources/masks/pista_debug_mask.png",
               {6900, 2657},
               0,
               3,
               {{/*Vector2*/ {4371, 2537}, 2.66f},
                {/*Vector2*/ {6700, 8147}, 0.0f},
                {/*Vector2*/ {11069, 2257}, 2.17f}}}};

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

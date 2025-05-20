#include "common.h"
#include "game.h"
#include "menu.h"

// Configurações de tela
int SCREEN_WIDTH  = 0;
int SCREEN_HEIGHT = 0;

// Condição de vitoria
int MAX_LAPS = 3;

// Suavidade da camera
float CAMERA_SMOOTHNESS = 0.05;

// Informações do jogo
char GAME_MODES[][100] = {"1 Jogador", "2 Jogadores"};
int  TOTAL_GAME_MODES  = sizeof(GAME_MODES) / sizeof(GAME_MODES[0]);

// Pasta de gravação dos arquivos dos carros fantasmas
char *GHOST_CAR_DATA_PATH = "./data/";

// Áudio
char *GAME_MUSIC_PATH         = "resources/sounds/game-music.mp3";
char *MENU_MUSIC_PATH         = "resources/sounds/menu-music.mp3";
char *CAR_SOUND_PATH          = "resources/sounds/f1s.mp3";
char *CLICK_BUTTON_SOUND_PATH = "resources/sounds/click.mp3";
char *SMAPHORE_SOUND_PATH     = "resources/sounds/click.mp3";
float CAR_VOLUME              = 0.0f;
float GAME_MUSIC_VOLUME       = 0.0f;
float MENU_MUSIC_VOLUME       = 0.00f;

// Recursos visuais
char *SEMAPHORE_PATH[]  = {"resources/others/nolight.png", "resources/others/redlight.png"};
char *BACKGROUND_PATH   = "resources/menu/menu.png";
char *CAR_IMAGES_PATH[] = {"resources/cars/branco.png", "resources/cars/azul.png",
                           "resources/cars/laranja.png"};
char *LOGO_IMAGE_PATH   = "resources/logo/formula_c-logo.png";
int   HUD_OPACITY       = 200;

// Mapas
Map MAPS[] = {
    {"Interlagos",
     "resources/maps/interlagos_map.png",   // backgroundPath
     "resources/masks/interlagos_mask.png", // maskPath
     "resources/minimaps/interlagos_minimap.png",
     {{4721, 1910}, {4900, 2061}},
     2.75f,
     10,
     {
         {/*Vector2*/ {4584, 2078}, 2.73f},
         {/*Vector2*/ {1816, 6076}, 1.75f},
         {/*Vector2*/ {3838, 8251}, 0.0f},
         {/*Vector2*/ {10380, 8239}, 0.0f},
         {/*Vector2*/ {9575, 5176}, -2.77f},
         {/*Vector2*/ {7315, 2106}, -0.63f},
         {/*Vector2*/ {9755, 2619}, -1.09f},
         {/*Vector2*/ {12303, 4781}, 0.07f},
         {/*Vector2*/ {14136, 3216}, -1.87f},
         {/*Vector2*/ {9357, 341}, -3.57f},
     }},
};

int TOTAL_MAPS = sizeof(MAPS) / sizeof(Map);

// Áreas da pista
TrackArea TRACK_AREAS[] = {
    /* pista  */ {{127, 127, 127}, 0.997},
    /* fraca  */ {{255, 127, 39}, 0.985},
    /* forte  */ {{163, 73, 164}, 0.98},
    /* grama  */ {{34, 177, 76}, 0.991}};

int TRACK_AREA_SIZE = sizeof(TRACK_AREAS) / sizeof(TrackArea);

// Cores da pista
Color OUTSIDE_TRACK_COLOR = {255, 255, 255};
Color CHECKPOINTS_COLOR   = {0, 255, 0};

// Carro padrão
CarConfig DEFAULT_CAR_CONFIG = {0.1, 0.2, 0.988, 0.035, 150, 75};

int WINNER_FONT_SIZE = 128;

// Estado do jogo
State state = {SINGLEPLAYER, MENU, false, INTERLAGOS, THIRD_PERSON};

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

    Menu_setup(Game_load);
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

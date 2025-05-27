#include "common.h"
#include "controller.h"
#include "game.h"
#include "menu.h"

//----------------------------------------------------------------------------------
// 🧠 VARIÁVEIS GLOBAIS
//----------------------------------------------------------------------------------

// 🎮 Estado e Configurações do Jogo
State     state              = {SINGLEPLAYER, MENU, false, INTERLAGOS, FIRST_PERSON, 0};
float     CAMERA_SMOOTHNESS  = 0.05;
int       HUD_OPACITY        = 200;
CarConfig DEFAULT_CAR_CONFIG = {
    0.1,   // Aceleração
    0.2,   // Força da macha ré
    0.988, // Força do Freio
    0.035, // Velocidade Angular
    150,   // Largura carro
    75     // Comprimento do carro
};

SDL_GameController *controllers[2];
int                 controllers_n = 0;

//----------------------------------------------------------------------------------
// 🔊 ÁUDIO
//----------------------------------------------------------------------------------

// 🎵 Música e Sons
char *GAME_MUSIC_PATH         = "resources/sounds/game-music.mp3";
char *MENU_MUSIC_PATH         = "resources/sounds/menu-music.mp3";
char *CAR_SOUND_PATH          = "resources/sounds/f1s.mp3";
char *CLICK_BUTTON_SOUND_PATH = "resources/sounds/click.mp3";
char *SMAPHORE_SOUND_PATH     = "resources/sounds/click.mp3";

float GAME_MUSIC_VOLUME = 0.02f;
float MENU_MUSIC_VOLUME = 0.005f;
float CAR_VOLUME        = 0.02f;

//----------------------------------------------------------------------------------
// 🗺️ MAPAS E PISTAS
//----------------------------------------------------------------------------------

// 🏁 Mapas e Pistas
Map MAPS[] = {{"Interlagos",                                // Nome da pista
               "resources/maps/interlagos_map.png",         // Path pista final
               "resources/masks/interlagos_mask.png",       // Path mask pista
               "resources/minimaps/interlagos_minimap.png", // Path para mini mapa
               {{4721, 1910}, {4900, 2061}},                // Inicial Spawn Car
               2.75f,                                       // Cars Spawn angle
               3,                                           // Max laps
               10,                                          // Quantidade de checkpoint
               {
                   // Checkpoint Spawn , posição e angulo
                   {{4584, 2078}, 2.73f},
                   {{1816, 6076}, 1.75f},
                   {{3838, 8251}, 0.0f},
                   {{10380, 8239}, 0.0f},
                   {{9575, 5176}, -2.77f},
                   {{7315, 2106}, -0.63f},
                   {{9755, 2619}, -1.09f},
                   {{12303, 4781}, 0.07f},
                   {{14136, 3216}, -1.87f},
                   {{9357, 341}, -3.57f},
               }},
              {"Monaco",
               "resources/maps/monaco_map.png",
               "resources/masks/monaco_mask.png",
               "resources/minimaps/monaco_minimap.png",
               {{2507, 3150}, {2360, 3158}},
               -0.90f,
               5,
               12,
               {
                   {{2617, 3104}, -0.90f},
                   {{5077, 2004}, 0.71f},
                   {{8014, 3815}, 0.43f},
                   {{10763, 3653}, -1.36f},
                   {{13396, 3463}, 1.95f},
                   {{14070, 3621}, 0.39f},
                   {{12910, 5363}, 2.60f},
                   {{8048, 4576}, 3.80f},
                   {{5841, 3037}, 3.84f},
                   {{2833, 4399}, 2.19f},
                   {{1746, 5504}, 1.92f},
                   {{1136, 5603}, 5.13f},
               }}};

int TOTAL_MAPS = sizeof(MAPS) / sizeof(Map);

TrackArea TRACK_AREAS[] = {
    {{127, 127, 127}, 0.997}, // pista
    {{255, 127, 39}, 0.985},  // fraca
    {{163, 73, 164}, 0.965},  // forte
    {{34, 177, 76}, 0.991},   // grama
};

int TRACK_AREA_SIZE = sizeof(TRACK_AREAS) / sizeof(TrackArea);

Color OUTSIDE_TRACK_COLOR = {255, 255, 255};
Color CHECKPOINTS_COLOR   = {0, 255, 0};

//----------------------------------------------------------------------------------
// 🎨 RECURSOS VISUAIS
//----------------------------------------------------------------------------------

// 🏎️ Carros
char *CAR_IMAGES_PATH[] = {
    "resources/cars/branco.png", // Carro branco
    "resources/cars/azul.png",   // Carro azul
    "resources/cars/laranja.png" // Carro laranja
};

char *SPEEDOMETER_PATH = "resources/cars/velocimetro.png";

// 🧠 Modos de Jogo
char GAME_MODES[][100] = {"1 Jogador", "2 Jogadores"};
int  TOTAL_GAME_MODES  = sizeof(GAME_MODES) / sizeof(GAME_MODES[0]);

// 🅰️ Interface e Imagens

char *BACKGROUND_PATH    = "resources/menu/menu.png";
char *LOGO_BG_IMAGE_PATH = "resources/logo/logo_background.png";
char *LOGO_IMAGE_PATH    = "resources/logo/formula_c-logo.png";

// 🔤 Fontes
char FONTS_PATH[][100] = {"resources/fonts/Formula-Regular.ttf", "resources/fonts/Formula-Bold.ttf",
                          "resources/fonts/Formula-Black.ttf",
                          "resources/fonts/Formula-Italic.ttf"};

int  FONTS_N = sizeof(FONTS_PATH) / sizeof(FONTS_PATH[0]);
Font FONTS[4];

//----------------------------------------------------------------------------------
// 📁 Arquivos de Dados
//----------------------------------------------------------------------------------
char *GHOST_CAR_DATA_PATH = "./data/best_laps/";
char *REFERENCE_DATA_PATH = "./data/references/";

//----------------------------------------------------------------------------------
// Início do jogo
//----------------------------------------------------------------------------------

int main() {
    if (GetMonitorCount() > 0) {
        SetConfigFlags(FLAG_FULLSCREEN_MODE);
        InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Formula C");
    } else {
        InitWindow(1920, 1080, "Formula C");
    }

    SCREEN_WIDTH  = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    Image icon = LoadImage(LOGO_IMAGE_PATH);
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);
    InitAudioDevice();

    for (int i = 0; i < FONTS_N; i++) {
        FONTS[i] = LoadFontEx(FONTS_PATH[i], 256, 0, 0);
    }

    SetTargetFPS(60);

    Menu_setup(Game_load);
    Game_setup();

    Controllers_init(controllers, &controllers_n);

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

    for (int i = 0; i < FONTS_N; i++) {
        UnloadFont(FONTS[i]);
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

#include "common.h"
#include "game.h"
#include "menu.h"

int SCREEN_WIDTH  = 0;
int SCREEN_HEIGHT = 0;

State state = {0};

int main() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Formula C");

    SCREEN_WIDTH  = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    Image icon = LoadImage("resources/logo/formula_c-logo.png");
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);

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

    CloseWindow(); // Fechar a janela gráfica 2d

    return 0;
}

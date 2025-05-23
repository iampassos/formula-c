#include "common.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

char strBuffer[1000];
int MAP_WIDTH;
int MAP_HEIGHT;

//----------------------------------------------------------------------------------
// Funções extras das structs da raylib
//----------------------------------------------------------------------------------

bool Color_equals(Color a, Color b) { // Verifica se uma cor é igual a outra
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

float Vector2_dist(Vector2 a, Vector2 b) {
    float deltaX = b.x - a.x;
    float deltaY = b.y - a.y;
    return sqrtf((deltaX * deltaX) + (deltaY * deltaY));
}

float LerpAngle(float a, float b, float t) { // Interpolando angulo
    float diff = fmodf(b - a + 180.0f, 360.0f) - 180.0f;
    return a + diff * t;
}

//----------------------------------------------------------------------------------
// Funções de desenho extras
//----------------------------------------------------------------------------------

void drawCenteredText(char *text, float x, float y, float width, float heigth, int size,
                      Color color, Font font) {
    Vector2 vec = MeasureTextEx(font, text, size, 1.0f);
    DrawTextEx(font, text, (Vector2) {((x * 2) + width - vec.x) / 2.0f, ((y * 2) + heigth) / 2.0f},
               size, 1.0f, color);
}

void drawTextWithShadow(char *text, float x, float y, int size, Color color, Font font) {
    DrawTextEx(font, text, (Vector2) {x + 1, y + 1}, size, 1.0f, BLACK);
    DrawTextEx(font, text, (Vector2) {x, y}, size, 1.0f, color);
}

void stringifyTime(char *buffer, double time, int signFlag) {
    int   mins = time / 60;
    float secs = time - (mins * 60);

    if (signFlag) {
        strBuffer[0] = time > 0 ? '+' : '-';
    }

    if (mins > 0) {
        snprintf(strBuffer + signFlag, 32, "%d:%06.3fs", mins, secs);
    } else {
        snprintf(strBuffer + signFlag, 32, "%05.3fs", secs);
    }

    strcpy(buffer, strBuffer);
}
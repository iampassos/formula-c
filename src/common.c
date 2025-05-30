#include "common.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

char strBuffer[2000];
int  MAP_WIDTH;
int  MAP_HEIGHT;

// 📐 Dimensões da Tela
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//----------------------------------------------------------------------------------
// Funções extras das structs da raylib
//----------------------------------------------------------------------------------

bool Color_equals(Color a, Color b) {
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

void drawTextCenteredInRect(char *text, Rectangle rect, int fontSize, Color color, Font font) {
    // DrawRectangle(rect.x, rect.y, rect.width, rect.height, BLUE); // Debug
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.0f);
    float   textX    = rect.x + (rect.width - textSize.x) / 2.0f;
    float   textY    = rect.y + (rect.height - textSize.y) / 2.0f;
    DrawTextEx(font, text, (Vector2) {textX, textY}, fontSize, 1.0f, color);
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

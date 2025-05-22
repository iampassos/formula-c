#include "common.h"
#include <math.h>

//----------------------------------------------------------------------------------
// Funções extras das structs da raylib
//----------------------------------------------------------------------------------

bool Color_equals(Color a, Color b) { // Verifica se uma cor é igual a outra
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

float Vector2_dist(Vector2 a, Vector2 b) {
    float deltaX = b.x - a.x;
    float deltaY = b.y - a.y;
    return sqrtf(deltaX * deltaX + deltaY * deltaY);
}

float LerpAngle(float a, float b, float t) {
    float diff = fmodf(b - a + 180.0f, 360.0f) - 180.0f;
    return a + diff * t;
}
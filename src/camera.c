#include "camera.h"
#include "car.h"
#include <math.h>
#include <stdlib.h>

static int BACKGROUND_WIDTH;
static int BACKGROUND_HEIGHT;

Camera2D *Camera_create(Vector2 target, Vector2 offset, float rotation, float zoom) {
    Camera2D *camera = (Camera2D *) malloc(sizeof(Camera2D));
    if (camera == NULL)
        return NULL;

    camera->target   = target;
    camera->offset   = offset;
    camera->rotation = rotation;
    camera->zoom     = zoom;

    return camera;
}

void Camera_free(Camera2D *camera) {
    free(camera);
}

void Camera_Background_setSize(int width, int height) {
    BACKGROUND_WIDTH  = width;
    BACKGROUND_HEIGHT = height;
}

static float LerpAngle(float a, float b, float t) {
    float diff = fmodf(b - a + 180.0f, 360.0f) - 180.0f;
    return a + diff * t;
}

void Camera_updateTarget(Camera2D *camera, Car *car) {
    float halfW = SCREEN_WIDTH / (2.0f * camera->zoom);
    float halfH = SCREEN_HEIGHT / (2.0f * camera->zoom);

    float x = car->pos.x;
    float y = car->pos.y;

    if (x < halfW)
        x = halfW;
    if (y < halfH)
        y = halfH;
    if (x > BACKGROUND_WIDTH - halfW)
        x = BACKGROUND_WIDTH - halfW;
    if (y > BACKGROUND_HEIGHT - halfH)
        y = BACKGROUND_HEIGHT - halfH;

    camera->target = (Vector2) {x, y};

    float targetZoom = 1.0f - (car->vel * 0.02f);

    if (targetZoom < 0.5f)
        targetZoom = 0.5f;
    if (targetZoom > 2.0f)
        targetZoom = 2.0f;

    camera->zoom += (targetZoom - camera->zoom) * 0.1f;
    if (state.cameraView == FIRST_PERSON) {
        float targetRotation = -car->angle * RAD2DEG - 90.0f;
        camera->rotation     = LerpAngle(camera->rotation, targetRotation, CAMERA_SMOOTHNESS);
    }
}
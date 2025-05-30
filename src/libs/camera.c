#include "camera.h"
#include "car.h"
#include "common.h"
#include <raylib.h>
#include <stdlib.h>

static int CAMERA_WIDTH;
static int CAMERA_HEIGHT;

static float START_ZOOM;

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

void Camera_setSize(int width, int height) {
    CAMERA_WIDTH  = width;
    CAMERA_HEIGHT = height;
    START_ZOOM    = state.mode == SPLITSCREEN ? 1.35f : 1.5f;
}

void Camera_updateTarget(Camera2D *camera, Car *car) {
    float halfW = CAMERA_WIDTH / (2.0f * camera->zoom);
    float halfH = CAMERA_HEIGHT / (2.0f * camera->zoom);

    float x = car->pos.x;
    float y = car->pos.y;

    if (x < halfW)
        x = halfW;
    if (y < halfH)
        y = halfH;
    if (x > MAP_WIDTH - halfW)
        x = MAP_WIDTH - halfW;
    if (y > MAP_HEIGHT - halfH)
        y = MAP_HEIGHT - halfH;

    camera->target = (Vector2) {x, y};

    float targetZoom = START_ZOOM - (car->vel / car->maxVelocity);
    camera->zoom += (targetZoom - camera->zoom) * 0.1f * GetFrameTime() * 60.0f;

    if (state.cameraView == FIRST_PERSON) {
        float targetRotation = -car->angle * RAD2DEG - 90.0f;
        camera->rotation =
            LerpAngle(camera->rotation, targetRotation, CAMERA_SMOOTHNESS * GetFrameTime() * 60.0f);
    }
}

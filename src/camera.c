#include "camera.h"
#include "car.h"
#include <stdlib.h>

static int BACKGROUND_WIDTH;
static int BACKGROUND_HEIGHT;

static int SCREEN_WIDTH;
static int SCREEN_HEIGHT;

Camera2D* Camera_create(Vector2 target, Vector2 offset, float rotation, float zoom){
    Camera2D* camera = (Camera2D*)malloc(sizeof(Camera2D));
    if (camera == NULL) return NULL;

    camera->target = target;
    camera->offset = offset;
    camera->rotation = rotation;
    camera->zoom = zoom;

    return camera;
}

void Camera_free(Camera2D* camera){
    free(camera);
}

void Camera_Background_setSize(int width, int height){
    BACKGROUND_WIDTH = width;
    BACKGROUND_HEIGHT = height;
}

void Camera_Screen_setSize(int width, int height){
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void Camera_updateTarget(Camera2D* camera, Car *car) {
    float halfW = SCREEN_WIDTH / (2.0f * camera->zoom);
    float halfH = SCREEN_HEIGHT / (2.0f * camera->zoom);

    float x = car->pos.x;
    float y = car->pos.y;

    if (x < halfW) x = halfW;
    if (y < halfH) y = halfH;
    if (x > BACKGROUND_WIDTH - halfW) x = BACKGROUND_WIDTH - halfW;
    if (y > BACKGROUND_HEIGHT - halfH) y = BACKGROUND_HEIGHT - halfH;

    camera->target = (Vector2){x, y};
}
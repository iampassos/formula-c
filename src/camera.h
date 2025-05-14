#ifndef CAMERA_H
#define CAMERA_H

#include "car.h"
#include "raylib.h"

Camera2D *Camera_create(Vector2 target, Vector2 offset, float rotation, float zoom);

void Camera_free(Camera2D *camera);

void Camera_Background_setSize(int width, int height);

void Camera_updateTarget(Camera2D *camera, Car *car);

#endif

#include "arrayList.h"
#include <stdlib.h>

ArrayList* ArrayList_create(){
    ArrayList* arr = (ArrayList*)malloc(sizeof(ArrayList));
    arr->length = 0;
    arr->capacity = 32;
    arr->data = (GhostCarFrame*)malloc(sizeof(GhostCarFrame) * arr->capacity);
    return arr;
}

void ArrayList_free(ArrayList* arr){
    free(arr->data);
    free(arr);
}

void ArrayList_growth(ArrayList* arr){
    arr->capacity = (arr->capacity >> 1) + arr->capacity;
    arr->data = realloc(arr->data, sizeof(GhostCarFrame) * arr->capacity);
}

void ArrayList_push(ArrayList* arr, GhostCarFrame car){
    if (arr->length == arr->capacity)
        ArrayList_growth(arr);
    arr->data[arr->length++] = car;
}

unsigned int ArrayList_length(ArrayList* arr){
    return arr->length;
}

void ArrayList_clear(ArrayList* arr){
    arr->length = 0;
}

void ArrayList_copy(ArrayList* a, ArrayList* b){
    a->length = b->length;
    a->capacity = b->length;

    a->data = realloc(a->data, sizeof(GhostCarFrame) * a->capacity);

    for (unsigned int i = 0; i < b->length; i++){
        a->data[i] = b->data[i];
    }
}

GhostCarFrame ArrayList_get(ArrayList* arr, unsigned int idx){
    if (idx >= arr->length)
        return (GhostCarFrame){0};
    return arr->data[idx];
}
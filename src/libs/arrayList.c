#include "arrayList.h"
#include <stdlib.h>

ArrayList *ArrayList_create() {
    ArrayList *array = (ArrayList *) malloc(sizeof(ArrayList));
    array->length    = 0;
    array->capacity  = 1024;
    array->data      = (CarFrame *) malloc(sizeof(CarFrame) * array->capacity);
    return array;
}

void ArrayList_free(ArrayList *array) {
    free(array->data);
    free(array);
}

void ArrayList_growth(ArrayList *array) {
    array->capacity += (array->capacity/2); // aumenta em escala exponencial menor que 2
    array->data     = realloc(array->data, sizeof(CarFrame) * array->capacity);
}

void ArrayList_push(ArrayList *array, CarFrame car) {
    if (array->length == array->capacity)
        ArrayList_growth(array);
    array->data[array->length++] = car;
}

unsigned int ArrayList_length(ArrayList *array) {
    return array->length;
}

void ArrayList_clear(ArrayList *array) {
    array->length = 0;
}

void ArrayList_copy(ArrayList *a, ArrayList *b) {
    a->length   = b->length;
    a->capacity = b->length;

    a->data = realloc(a->data, sizeof(CarFrame) * a->capacity);

    for (unsigned int i = 0; i < b->length; i++) {
        a->data[i] = b->data[i];
    }
}

CarFrame ArrayList_get(ArrayList *array, unsigned int idx) {
    if (idx >= array->length)
        return (CarFrame) {0};
    return array->data[idx];
}

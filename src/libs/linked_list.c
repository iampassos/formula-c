#include "linked_list.h"
#include <stdlib.h>

LinkedList *LinkedList_create() {
    LinkedList *list = (LinkedList *) malloc(sizeof(LinkedList));
    if (list == NULL)
        return NULL;
    list->head   = NULL;
    list->length = 0;
    return list;
}

void LinkedList_free(LinkedList *list) {
    Node *cur = list->head;
    Node *temp;

    while (cur != NULL) {
        temp = cur;
        cur  = cur->next;
        Car_free(temp->car);
        free(temp);
    }
    free(list);
}

void LinkedList_clear(LinkedList *list) {
    Node *cur = list->head;
    Node *temp;

    while (cur != NULL) {
        temp = cur;
        cur  = cur->next;
        Car_free(temp->car);
        free(temp);
    }
    list->head   = NULL;
    list->length = 0;
}

int LinkedList_addCar(LinkedList *list, Car *car) {
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL)
        return 0;
    node->car  = car;
    node->next = list->head;
    list->head = node;
    list->length++;
    return 1;
}

Car *LinkedList_removeCarById(LinkedList *list, int id) {
    Node *cur  = list->head;
    Node *last = NULL;

    while (cur != NULL && cur->car->id != id) {
        last = cur;
        cur  = cur->next;
    }

    if (cur == NULL)
        return NULL;
    Car *car = cur->car;

    if (last == NULL) {
        list->head = cur->next;
    } else {
        last->next = cur->next;
    }

    free(cur);
    list->length--;
    return car;
}

int LinkedList_size(LinkedList *list) {
    return list->length;
}

Car *LinkedList_getCarById(LinkedList *list, int id) {
    Node *cur = list->head;

    while (cur != NULL && cur->car->id != id) {
        cur = cur->next;
    }

    if (cur == NULL)
        return NULL;
    return cur->car;
}

static void swap(Node *a, Node *b) {
    Car *temp = a->car;
    a->car    = b->car;
    b->car    = temp;
}

void LinkedList_sort(LinkedList *list, Car_compare function) {
    Node *cur = list->head;
    Node *temp;

    while (cur != NULL) {
        temp = cur->next;
        while (temp != NULL) {
            if (function(cur->car, temp->car) < 0)
                swap(cur, temp);
            temp = temp->next;
        }
        cur = cur->next;
    }
}

void LinkedList_forEach(LinkedList *list, Car_function function) {
    Node *cur = list->head;

    while (cur != NULL) {
        function(cur->car);
        cur = cur->next;
    }
}

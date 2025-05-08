#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>

LinkedList* LinkedList_create(){
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) return NULL;
    list->head = list->tail = NULL;
    list->length = 0;
    return list;
}

void LinkedList_free(LinkedList* list){
    Node* cur = list->head;
    Node* temp;

    while(cur != NULL){
        temp = cur;
        cur = cur->next;
        Car_free(temp->car);
        free(temp);
    }
    free(list);
}

void LinkedList_clear(LinkedList* list){
    Node* cur = list->head;
    Node* temp;

    while(cur != NULL){
        temp = cur;
        cur = cur->next;
        Car_free(temp->car);
        free(temp);
    }
    list->head = list->tail = NULL;
    list->length = 0;
}

int LinkedList_addLast(LinkedList* list, Car* car){
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) return 0;
    node->car = car;
    node->next = NULL;
    node->previous = list->tail;

    if (list->tail == NULL){
        list->head = node;
    } else {
        list->tail->next = node;
    }

    list->tail = node;
    list->length++;
    return 1;
}

int LinkedList_addFirst(LinkedList* list, Car* car){
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) return 0;
    node->car = car;
    node->next = list->head;
    node->previous = NULL;

    if (list->head == NULL){
        list->tail = node;
    } else {
        list->head->previous = node;
    }

    list->head = node;
    list->length++;
    return 1;
}

Car* LinkedList_removeLast(LinkedList* list){
    if (list->tail == NULL) return NULL;

    Node* last = list->tail;
    Car* car = last->car;

    list->tail = last->previous;

    if (list->tail == NULL){
        list->head = NULL;
    } else {
        list->tail->next = NULL;
    }
    list->length--;
    free(last);
    return car;
}

Car* LinkedList_removeFirst(LinkedList* list){
    if (list->head == NULL) return NULL;

    Node* first = list->head;
    Car* car = first->car;

    list->head = first->next;

    if (list->head == NULL){
        list->tail = NULL;
    } else {
        list->head->previous = NULL;
    }
    list->length--;
    free(first);
    return car;
}

Car* LinkedList_removeId(LinkedList* list, int id){
    Node* cur = list->head;

    while(cur != NULL && cur->car->id != id){
        cur = cur->next;
    }

    if (cur == NULL) return NULL;
    Car* car = cur->car;

    if (cur->previous == NULL){
        list->head = list->head->next;
        if (list->head != NULL)
            list->head->previous = NULL;
        else
            list->tail = NULL;
    } else {
        cur->previous->next = cur->next;
        if (cur->next != NULL)
            cur->next->previous = cur->previous;
    }

    free(cur);
    list->length--;
    return car;
}

Car* LinkedList_peakFirst(LinkedList* list){
    if (list->head == NULL) return NULL;
    return list->head->car;
}

Car* LinkedList_peakLast(LinkedList* list){
    if (list->tail == NULL) return NULL;
    return list->tail->car;
}

int LinkedList_size(LinkedList* list){
    return list->length;
}

void LinkedList_print(LinkedList* list){
    printf("{");
    Node* cur = list->head;
    while(cur != NULL){
        printf("[%d]", cur->car->id);
        cur = cur->next;
        if (cur != NULL)
            printf("<->");
    }
    printf("}\n");
}

Car* LinkedList_search(LinkedList* list, int id){
    Node* cur = list->head;

    while (cur != NULL && cur->car->id != id){
        cur = cur->next;
    }
    
    if (cur == NULL) return NULL;
    return cur->car;
}

void swap(Node* a, Node* b){
    Car *temp = a->car;
    a->car = b->car;
    b->car = temp;
}

void LinkedList_sort(LinkedList* list){
    Node* cur = list->head;
    Node* temp;

    while(cur != NULL){
        temp = cur->next;
        while(temp != NULL){
            if (cur->car->lapTime > temp->car->lapTime)
                swap(cur, temp);
            temp = temp->next;
        }
        cur = cur->next;
    }
}

void LinkedList_forEach(LinkedList* list, void (*function)(Car*)){
    Node* cur = list->head;

    while(cur != NULL){
        function(cur->car);
        cur = cur->next;
    }
}
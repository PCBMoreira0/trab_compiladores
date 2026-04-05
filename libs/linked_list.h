#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    void *data;
    struct Node* next;
} LinkedList;

LinkedList* createList();
LinkedList* insertAtHead(LinkedList* head, void *data, size_t data_size);
void freeList(LinkedList* head);
void printList(struct Node *head, void (*fptr)(void *));
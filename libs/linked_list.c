#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

LinkedList *createList(){
    return NULL;
}

LinkedList* insertAtHead(LinkedList* head, void *data, size_t data_size) {
    LinkedList* newLinkedList = (LinkedList*)malloc(sizeof(LinkedList));
    if (!newLinkedList) return NULL;

    newLinkedList->data = malloc(data_size);
    if (!newLinkedList->data) {
        free(newLinkedList);
        return NULL;
    }
    memcpy(newLinkedList->data, data, data_size);
    
    newLinkedList->next = head;
    return newLinkedList;
}

void freeList(LinkedList* head) {
    LinkedList* tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp->data);
        free(tmp);
    }
}

void printList(LinkedList *head, void (*fptr)(void *)) {
    while (head != NULL) {
        (*fptr)(head->data);
        head = head->next;
    }
}
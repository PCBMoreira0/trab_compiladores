#include <stdio.h>
#include <stdlib.h>
#include "libs/linked_list.h"


Node* create_node(AFN_State *data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        printf("Erro ao alocar memória\n");
        exit(1);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void insert_front(Node** head, AFN_State *data) {
    Node* new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}

void remove_node(Node** head, AFN_State *data) {
    Node* current = *head;
    Node* prev = NULL;

    while (current) {
        if (current->data == data) {
            if (prev) {
                prev->next = current->next;
            } else {
                *head = current->next; // Remove o primeiro elemento
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// void print_list(Node* head) {
//     Node* current = head;
//     while (current) {
//         printf("%d -> ", current->data);
//         current = current->next;
//     }
//     printf("NULL\n");
// }

void free_list(Node* head) {
    Node* tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}
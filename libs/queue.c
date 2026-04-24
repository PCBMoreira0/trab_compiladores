#include <stdlib.h>
#include "queue.h"

typedef struct Node {
    const void* data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

Queue* queue_create() {
    Queue* q = (Queue*) malloc(sizeof(Queue));
    if (q) {
        q->front = NULL;
        q->rear = NULL;
    }
    return q;
}

void queue_enqueue(Queue* q, const void* data) {
    if (!q) return;

    Node* node = (Node*) malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;

    if (q->rear == NULL) {
        // fila vazia
        q->front = node;
        q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
}

const void* queue_dequeue(Queue* q) {
    if (q == NULL || q->front == NULL) return NULL;

    Node* temp = q->front;
    const void* data = temp->data;

    q->front = temp->next;

    if (q->front == NULL) {
        // ficou vazia
        q->rear = NULL;
    }

    free(temp);
    return data;
}

const void* queue_peek(Queue* q) {
    if (q == NULL || q->front == NULL) return NULL;
    return q->front->data;
}

int queue_is_empty(Queue* q) {
    return (q == NULL || q->front == NULL);
}

int queue_size(Queue* q) {
    if (q == NULL) return 0;

    int count = 0;
    Node* current = q->front;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

void queue_destroy(Queue* q) {
    if (!q) return;

    while (q->front != NULL) {
        Node* temp = q->front;
        q->front = temp->next;
        free(temp);
    }

    free(q);
}
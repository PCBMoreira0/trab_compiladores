#pragma once

typedef struct Queue Queue;

Queue* queue_create();

void queue_enqueue(Queue* q, const void* data);

const void* queue_dequeue(Queue* q);

const void* queue_peek(Queue* q);

int queue_is_empty(Queue* q);

int queue_size(Queue* q);

void queue_destroy(Queue* q);
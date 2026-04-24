#include <stdlib.h>
#include <string.h>
#include "value_stack.h"

// O nó usa um "Flexible Array Member" (C99)
// Isso permite que 'data' ocupe o espaço logo após o ponteiro 'next'
typedef struct Node {
    struct Node* next;
    unsigned char data[]; 
} Node;

typedef struct ValueStack {
    Node* top;
    size_t element_size; // Tamanho de cada item armazenado
} ValueStack;

// Agora você informa o tamanho do item ao criar a pilha
ValueStack* value_stack_create(size_t element_size) {
    ValueStack* s = (ValueStack*) malloc(sizeof(ValueStack));
    if (s) {
        s->top = NULL;
        s->element_size = element_size;
    }
    return s;
}

// Passamos o endereço da variável, mas a pilha faz uma CÓPIA do valor
void value_stack_push(ValueStack* s, const void* value_ptr) {
    if (!s || !value_ptr) return;
    
    // Aloca o tamanho do nó + o espaço para o dado
    Node* node = (Node*) malloc(sizeof(Node) + s->element_size);
    if (!node) return;

    // Copia o valor para dentro do nó
    memcpy(node->data, value_ptr, s->element_size);
    
    node->next = s->top;
    s->top = node;
}

// Para retirar, copiamos o valor para a variável de destino (out_data)
bool value_stack_pop(ValueStack* s, void* out_data) {
    if (s == NULL || s->top == NULL || out_data == NULL) return false;

    Node* temp = s->top;
    
    // Copia o dado do nó de volta para a variável do usuário
    memcpy(out_data, temp->data, s->element_size);

    s->top = temp->next;
    free(temp);

    return true; // Sucesso
}

// Opcional: Espiar o topo sem remover (retorna um ponteiro para a cópia interna)
void* value_stack_peek(ValueStack* s) {
    if (s == NULL || s->top == NULL) return NULL;
    return s->top->data;
}

int value_stack_is_empty(ValueStack* s) {
    return (s == NULL || s->top == NULL);
}

void value_stack_destroy(ValueStack* s) {
    if (!s) return;
    while (s->top != NULL) {
        Node* temp = s->top;
        s->top = temp->next;
        free(temp);
    }
    free(s);
}
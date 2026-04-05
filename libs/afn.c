#include <stdlib.h>
#include <stdio.h>
#include "afn.h"

// A variável global_state_id foi completamente removida.

/* ==========================================
 * GERENCIAMENTO DE CONTEXTO
 * ========================================== */

AFN_Context* afnCreateContext() {
    AFN_Context *ctx = (AFN_Context *)malloc(sizeof(AFN_Context));
    if (!ctx) return NULL;
    
    ctx->state_count = 0;
    ctx->capacity = 16;
    ctx->allocated_states = (AFN_State **)malloc(ctx->capacity * sizeof(AFN_State*));
    
    return ctx;
}

void afnFreeContext(AFN_Context *ctx) {
    if (!ctx) return;

    for (int i = 0; i < ctx->state_count; i++) {
        AFN_State *s = ctx->allocated_states[i];
        
        Node *curr = s->transitions;
        while (curr) {
            Node *next = curr->next;
            free(curr);
            curr = next;
        }
        free(s);
    }
    free(ctx->allocated_states);
    free(ctx);
}

/* ==========================================
 * CRIAÇÃO DE ESTADOS E TRANSIÇÕES
 * ========================================== */

AFN_State* afnNewState(AFN_Context *ctx, int isEndState) {
    AFN_State *newState = (AFN_State *)malloc(sizeof(AFN_State));
    if (!newState) return NULL;

    newState->id = ctx->state_count; 
    newState->isEndState = isEndState;
    newState->transitions = NULL;

    if (ctx->state_count >= ctx->capacity) {
        int new_capacity = ctx->capacity * 2;
        AFN_State **temp = (AFN_State **)realloc(
            ctx->allocated_states, 
            new_capacity * sizeof(AFN_State*)
        );
        
        if (!temp) {
            free(newState); 
            return NULL;
        }
        ctx->allocated_states = temp;
        ctx->capacity = new_capacity;
    }

    ctx->allocated_states[ctx->state_count] = newState;
    ctx->state_count++;

    return newState;
}

void afnAddTransition(AFN_State *from, char transitionChar, AFN_State *to) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) return;

    newNode->transitionChar = transitionChar;
    newNode->to = to;
    
    newNode->next = from->transitions;
    from->transitions = newNode;
}

/* ==========================================
 * CONSTRUTORES DO ALGORITMO DE THOMPSON
 * ========================================== */

AFN_Fragment afnCreateSymbol(AFN_Context *ctx, char character) {
    AFN_State *start = afnNewState(ctx, 0);
    AFN_State *end = afnNewState(ctx, 1);

    afnAddTransition(start, character, end);

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnCreateUnion(AFN_Context *ctx, AFN_Fragment a, AFN_Fragment b) {
    AFN_State *start = afnNewState(ctx, 0);
    AFN_State *end = afnNewState(ctx, 1);

    afnAddTransition(start, EPSILON_CHAR, a.start);
    afnAddTransition(start, EPSILON_CHAR, b.start);

    afnAddTransition(a.end, EPSILON_CHAR, end);
    afnAddTransition(b.end, EPSILON_CHAR, end);

    a.end->isEndState = 0;
    b.end->isEndState = 0;

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnCreateConcat(AFN_Context *ctx, AFN_Fragment a, AFN_Fragment b) {
    afnAddTransition(a.end, EPSILON_CHAR, b.start);
    a.end->isEndState = 0;

    AFN_Fragment frag = {a.start, b.end};
    return frag;
}

AFN_Fragment afnCreateKleene(AFN_Context *ctx, AFN_Fragment a) {
    AFN_State *start = afnNewState(ctx, 0);
    AFN_State *end = afnNewState(ctx, 1);

    afnAddTransition(start, EPSILON_CHAR, a.start);
    afnAddTransition(start, EPSILON_CHAR, end);
    afnAddTransition(a.end, EPSILON_CHAR, a.start);
    afnAddTransition(a.end, EPSILON_CHAR, end);
    a.end->isEndState = 0;

    AFN_Fragment frag = {start, end};
    return frag;
}

/* ==========================================
 * UTILITÁRIOS (DEBUG)
 * ========================================== */

static void afnPrintHelper(AFN_State *state, int *visited) {
    if (!state || visited[state->id]) return;
    
    visited[state->id] = 1;
    printf("Estado %d %s:\n", state->id, state->isEndState ? "[FINAL]" : "");
    Node *curr = state->transitions;
    if (!curr) {
        printf("  (sem transições)\n");
    }
    while (curr) {
        char c = (curr->transitionChar == EPSILON_CHAR) ? 'E' : curr->transitionChar;
        printf("  --(%c)--> Estado %d\n", c, curr->to->id);
        curr = curr->next;
    }
    curr = state->transitions;
    while (curr) {
        afnPrintHelper(curr->to, visited);
        curr = curr->next;
    }
}

void afnPrint(AFN_Context *ctx, AFN_State *start) {
    if (!start || !ctx) return;
    printf("--- ESTRUTURA DO AFN ---\n");

    int *visited = (int *)calloc(ctx->state_count, sizeof(int));
    afnPrintHelper(start, visited);
    free(visited);
    
    printf("------------------------\n\n");
}
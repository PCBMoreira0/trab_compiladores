#include <stdlib.h>
#include <stdio.h>
#include "afn.h"

#define MAX_TRANSITIONS 10
#define EMPTY_STATE_CHAR '\0'
#define EPSILON_CHAR '\0'

typedef struct Node Node;

typedef struct AFN_State{
    int isEndState;
    AFN_State **nextStates;
    char *transitions;
    int transitionCount;
} AFN_State;

AFN_State *afnNewState(int isEndState) {
    AFN_State *newNode = malloc(sizeof(AFN_State));
    newNode->isEndState = isEndState;
    newNode->nextStates = malloc(sizeof(AFN_State*) * MAX_TRANSITIONS);
    newNode->transitions = malloc(sizeof(char) * MAX_TRANSITIONS);
    newNode->transitionCount = 0;
    return newNode;
}

AFN_State *afnAddTransition(AFN_State *from, AFN_State *to, char character) {
    if(from->transitionCount >= MAX_TRANSITIONS) {
        // Handle error: too many transitions
        return NULL;
    }
    from->nextStates[from->transitionCount] = to;
    from->transitions[from->transitionCount] = character;
    from->transitionCount++;
    return from;
}

AFN_State *afnCreateSymbol(char character){
    AFN_State *start = afnNewState(0);
    AFN_State *end = afnNewState(1);
    afnAddTransition(start, end, character);

    return start;
}

AFN_State *afnCreateUnion(AFN_State *a, AFN_State *b) {
    AFN_State *start = afnNewState(0);
    AFN_State *end = afnNewState(1);

    afnAddTransition(start, a, EPSILON_CHAR);
    afnAddTransition(start, b, EPSILON_CHAR);
    afnAddTransition(a, end, EPSILON_CHAR);
    afnAddTransition(b, end, EPSILON_CHAR);

    return start;
}

AFN_State *afnCreateKleene(AFN_State *a) {
    AFN_State *start = afnNewState(0);
    AFN_State *end = afnNewState(1);

    afnAddTransition(start, a, EPSILON_CHAR);
    afnAddTransition(start, end, EPSILON_CHAR);
    afnAddTransition(a, a, EPSILON_CHAR);
    afnAddTransition(a, end, EPSILON_CHAR);

    return start;
}

void afnFree(AFN_State *state) {
    if(state == NULL) return;
    for(int i = 0; i < state->transitionCount; i++) {
        afnFree(state->nextStates[i]);
    }
    free(state->nextStates);
    free(state->transitions);
    free(state);
}

// --- FUNÇÃO AUXILIAR ---
// Verifica se um estado já está na lista de visitados
int isVisited(AFN_State **visited, int visitedCount, AFN_State *state) {
    for (int i = 0; i < visitedCount; i++) {
        if (visited[i] == state) return 1; // Já foi visitado
    }
    return 0; // Não foi visitado
}

// --- IMPRESSÃO ---
void afnPrintRecursive(AFN_State *state, int depth, AFN_State **visited, int *visitedCount) {
    if(state == NULL) return;

    for(int i = 0; i < depth; i++) printf("  ");

    // Se já visitou, imprime apenas o destino para não entrar em loop infinito
    if (isVisited(visited, *visitedCount, state)) {
        printf("State %p (already printed)\n", (void*)state);
        return;
    }

    // Adiciona aos visitados
    visited[*visitedCount] = state;
    (*visitedCount)++;

    printf("State %p (isEndState: %d)\n", (void*)state, state->isEndState);

    for(int i = 0; i < state->transitionCount; i++) {
        for(int j = 0; j < depth + 1; j++) printf("  ");
        
        // Melhora a visualização do caractere vazio (Epsilon)
        char tChar = state->transitions[i] == '\0' ? 'E' : state->transitions[i];
        printf("Transition on '%c' to %p\n", tChar, (void*)state->nextStates[i]);
        
        // Chamada recursiva
        afnPrintRecursive(state->nextStates[i], depth + 2, visited, visitedCount);
    }
}

void afnPrint(AFN_State *state, int depth) {
    AFN_State *visited[1000]; // Array simples para rastrear até 1000 estados
    int visitedCount = 0;
    printf("\n--- AFN Graph ---\n");
    afnPrintRecursive(state, depth, visited, &visitedCount);
}
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

void afnPrint(AFN_State *state, int depth) {
    if(state == NULL) return;
    for(int i = 0; i < depth; i++) printf("  ");
    printf("State (isEndState: %d)\n", state->isEndState);
    for(int i = 0; i < state->transitionCount; i++) {
        for(int j = 0; j < depth + 1; j++) printf("  ");
        printf("Transition on '%c'\n", state->transitions[i]);
        afnPrint(state->nextStates[i], depth + 2);
    }
}
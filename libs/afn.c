#include <stdlib.h>
#include "libs/afn.h"

#define MAX_TRANSITIONS 10
#define EMPTY_STATE_CHAR '\0'
#define EPSILON_CHAR '\0'

typedef struct Node Node;

typedef struct AFN_State{
    int isEndState;
    char transitionChar;
    Node *list;
} AFN_State;

AFN_State *afnNewEmptyState(int isEndState) {
    AFN_State *newNode = malloc(sizeof(AFN_State));
    newNode->isEndState = isEndState;
    newNode->transitionChar = EPSILON_CHAR;
    newNode->list = NULL;
    return newNode;
}

AFN_State *afnNewState(char transitionChar, int isEndState) {
    AFN_State *newNode = malloc(sizeof(AFN_State));
    newNode->isEndState = isEndState;
    newNode->transitionChar = transitionChar;
    newNode->list = NULL;
    return newNode;
}

AFN_State *afnAddTransition(AFN_State *from, AFN_State *to) {
    insert_front(&from->list, to);
    return from;
}

AFN_State *afnCreateSymbol(char character){
    AFN_State *start = afnNewEmptyState(0);
    AFN_State *end = afnNewEmptyState(1);
    AFN_State *state = afnNewState(character, 0);  
    
    afnAddTransition(start, state);
    afnAddTransition(state, end);

    return start;
}

AFN_State *afnCreateUnion(AFN_State *a, AFN_State *b) {
    AFN_State *start = afnNewEmptyState(0);
    AFN_State *end = afnNewEmptyState(1);

    afnAddTransition(start, a);
    afnAddTransition(start, b);
    afnAddTransition(a, end);
    afnAddTransition(b, end);

    return start;
}
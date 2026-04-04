#include <stdlib.h>
#include <stdio.h>
#include "afn.h"

static int global_state_id = 0;

AFN_State *afnNewState(int isEndState) {
    AFN_State *newState = (AFN_State *)malloc(sizeof(AFN_State));
    if (!newState) return NULL;

    newState->id = global_state_id++;
    newState->isEndState = isEndState;
    newState->transitions = NULL;
    
    return newState;
}

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

void afnPrint(AFN_State *start) {
    if (!start) return;
    printf("--- ESTRUTURA DO AFN ---\n");
    int *visited = (int *)calloc(global_state_id, sizeof(int));
    afnPrintHelper(start, visited);
    free(visited);
    printf("------------------------\n\n");
}

static void afnFreeHelper(AFN_State *state, int *visited, AFN_State **state_array) {
    if (!state || visited[state->id]) return;
    visited[state->id] = 1;
    state_array[state->id] = state;
    Node *curr = state->transitions;
    while (curr) {
        afnFreeHelper(curr->to, visited, state_array);
        curr = curr->next;
    }
}

void afnFree(AFN_State *start) {
    if (!start) return;

    int *visited = (int *)calloc(global_state_id, sizeof(int));
    AFN_State **state_array = (AFN_State **)calloc(global_state_id, sizeof(AFN_State*));

    afnFreeHelper(start, visited, state_array);
    for (int i = 0; i < global_state_id; i++) {
        AFN_State *s = state_array[i];
        
        if (s) {
            Node *curr = s->transitions;
            while (curr) {
                Node *next = curr->next;
                free(curr);
                curr = next;
            }
            free(s);
        }
    }
    free(visited);
    free(state_array);
}

void afnAddTransition(AFN_State *from, char transitionChar, AFN_State *to) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) return;

    newNode->transitionChar = transitionChar;
    newNode->to = to;
    
    newNode->next = from->transitions;
    from->transitions = newNode;
}

AFN_Fragment afnCreateSymbol(char character) {
    AFN_State *start = afnNewState(0);
    AFN_State *end = afnNewState(1);

    afnAddTransition(start, character, end);

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnCreateUnion(AFN_Fragment a, AFN_Fragment b) {
    AFN_State *start = afnNewState(0);
    AFN_State *end = afnNewState(1);

    afnAddTransition(start, EPSILON_CHAR, a.start);
    afnAddTransition(start, EPSILON_CHAR, b.start);

    afnAddTransition(a.end, EPSILON_CHAR, end);
    afnAddTransition(b.end, EPSILON_CHAR, end);

    a.end->isEndState = 0;
    b.end->isEndState = 0;

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnCreateConcat(AFN_Fragment a, AFN_Fragment b) {
    afnAddTransition(a.end, EPSILON_CHAR, b.start);
    a.end->isEndState = 0;

    AFN_Fragment frag = {a.start, b.end};
    return frag;
}

AFN_Fragment afnCreateKleene(AFN_Fragment a) {
    AFN_State *start = afnNewState(0);
    AFN_State *end = afnNewState(1);

    afnAddTransition(start, EPSILON_CHAR, a.start);
    afnAddTransition(start, EPSILON_CHAR, end);
    afnAddTransition(a.end, EPSILON_CHAR, a.start);
    afnAddTransition(a.end, EPSILON_CHAR, end);
    a.end->isEndState = 0;

    AFN_Fragment frag = {start, end};
    return frag;
}
#pragma once

#include "afn.h"

#define ALPHABET_SIZE 256 
#define STATE_DEAD -1

typedef struct {
    int initial_state;
    int num_states;
    int *is_final;
    int **transitions;
} Automato_AFD;

int run_dfa(Automato_AFD *dfa, const char *text);

int* afdCreateSignature(AFN_Context *ctx, LinkedList *set);
int afdFindSignatureIndex(AFN_Context *ctx, int *new_signature, int **known_signatures, int known_count);

Automato_AFD* afdBuild(AFN_Context *ctx, AFN_State *start_nfa_state);

void afdFree(Automato_AFD *dfa);
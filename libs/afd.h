#pragma once
#define ALPHABET_SIZE 256 
#define STATE_DEAD -1

typedef struct {
    int num_states;
    int initial_state;
    int *is_final_state;    
    int **transition_table; 
} Automato_AFD;

int run_dfa(Automato_AFD *dfa, const char *text);
#include <stdlib.h>
#include "afd.h"

int run_dfa(Automato_AFD *dfa, const char *text) {
    int current_state = dfa->initial_state;
    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = text[i];
        current_state = dfa->transition_table[current_state][c]; 
        if (current_state == STATE_DEAD) {
            return 0; 
        }
    }
    return dfa->is_final_state[current_state];
}
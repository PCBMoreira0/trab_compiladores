#pragma once

#include "regular_expression.h"
#include "afd.h"

typedef struct {
    ERTokenType type;
    char *lexeme;
    int line;
    int column;
} Token;

typedef struct {
    Token *tokens;
    int count;
    int capacity;
} TokenList;

TokenList* tokenize_file(Automato_AFD *dfa, const char *filename);
void free_token_list(TokenList *list);
void print_token_list(TokenList *list);
void save_token_list_to_file(TokenList *list, const char *filename);
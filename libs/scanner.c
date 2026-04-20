#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "lex_parsing.h"

char* read_file_to_string(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);
    return buffer;
}

TokenList* tokenize_file(Automato_AFD *dfa, const char *filename) {
    char *source = read_file_to_string(filename);
    if (!source) {
        printf("Erro: Nao foi possivel abrir o arquivo %s\n", filename);
        return NULL;
    }

    TokenList *list = malloc(sizeof(TokenList));
    list->capacity = 100;
    list->count = 0;
    list->tokens = malloc(sizeof(Token) * list->capacity);

    int cursor = 0;
    int line = 1;
    int col = 1;

    while (source[cursor] != '\0') {
        int current_state = dfa->initial_state;
        int last_accepted_state = -1;
        int last_accepted_pos = -1;
        ERTokenType last_accepted_type = TOKEN_EMPTY;

        int i = cursor;
        
        while (source[i] != '\0') {
            unsigned char c = source[i];
            current_state = dfa->transitions[current_state][c];

            if (current_state == STATE_DEAD) {
                break;
            }

            if (dfa->is_final[current_state] != TOKEN_EMPTY) { 
                last_accepted_state = current_state;
                last_accepted_pos = i;
                last_accepted_type = dfa->is_final[current_state];
            }
            i++;
        }

        Token t;
        t.line = line;
        t.column = col;

        if (last_accepted_pos != -1) {
            int len = last_accepted_pos - cursor + 1;
            
            t.lexeme = malloc(len + 1);
            strncpy(t.lexeme, &source[cursor], len);
            t.lexeme[len] = '\0';
            t.type = last_accepted_type;

            for (int k = cursor; k <= last_accepted_pos; k++) {
                if (source[k] == '\n') { line++; col = 1; }
                else { col++; }
            }
            cursor = last_accepted_pos + 1;
        } else {
            t.lexeme = malloc(2);
            t.lexeme[0] = source[cursor];
            t.lexeme[1] = '\0';
            t.type = TOKEN_EMPTY;
            col++;
            cursor++;
        }

        if (t.type != TOKEN_WHITESPACE) {
            if (list->count >= list->capacity) {
                list->capacity *= 2;
                list->tokens = realloc(list->tokens, sizeof(Token) * list->capacity);
            }
            list->tokens[list->count++] = t;
        } else {
            free(t.lexeme);
        }
    }

    Token eof_token = {TOKEN_EOF, strdup("EOF"), line, col};
    if (list->count >= list->capacity) {
        list->capacity++;
        list->tokens = realloc(list->tokens, sizeof(Token) * list->capacity);
    }
    list->tokens[list->count++] = eof_token;

    free(source);
    return list;
}

void print_token_list(TokenList *list) {
    printf("\n--- FITA DE TOKENS GERADA ---\n");
    for (int i = 0; i < list->count; i++) {
        Token t = list->tokens[i];
        printf("[Ln %02d, Col %02d] Tipo: %s | Lexema: '%s'\n", 
               t.line, t.column, get_token_name(t.type), t.lexeme);
    }
    printf("-----------------------------\n");
}

void free_token_list(TokenList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->tokens[i].lexeme);
    }
    free(list->tokens);
    free(list);
}

void save_token_list_to_file(TokenList *list, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Erro ao criar arquivo de saida de tokens: %s\n", filename);
        return;
    }

    for (int i = 0; i < list->count; i++) {
        Token t = list->tokens[i];
        fprintf(f, "%d %d %d %zu %s\n", 
                t.type, 
                t.line, 
                t.column, 
                strlen(t.lexeme), 
                t.lexeme);
    }

    fclose(f);
    printf("[+] Fita de tokens salva com sucesso em: %s\n", filename);
}
#pragma once

typedef enum ERTokenType {
    TOKEN_EMPTY,
    TOKEN_IF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER
} ERTokenType;

typedef struct ERToken {
    char *value; 
    ERTokenType type; 
    int priority;
} ERToken;

char *shuntingYard(const char *expression);
char *ERpreProcess(char *expression);
#pragma once

typedef enum ERTokenType {
    TOKEN_EMPTY = 0,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_DOT,
    TOKEN_KW_EXPRESSION,
    TOKEN_KW_MODULE_STAR,
    TOKEN_KW_MODULE,
    TOKEN_KW_PLAIN_MOD_BEGIN,
    TOKEN_KW_BEGIN_FOR_SYNTAX,
    TOKEN_KW_BEGIN0,
    TOKEN_KW_BEGIN,
    TOKEN_KW_PROVIDE,
    TOKEN_KW_DECLARE,
    TOKEN_KW_DEFINE_VALUES,
    TOKEN_KW_DEFINE_SYNTAXES,
    TOKEN_KW_REQUIRE,
    TOKEN_KW_PLAIN_LAMBDA,
    TOKEN_KW_CASE_LAMBDA,
    TOKEN_KW_IF,
    TOKEN_KW_LETREC_VALUES,
    TOKEN_KW_LET_VALUES,
    TOKEN_KW_SET,
    TOKEN_KW_QUOTE_SYNTAX,
    TOKEN_KW_QUOTE,
    TOKEN_KW_LOCAL,
    TOKEN_KW_WITH_CONT_MARK,
    TOKEN_KW_PLAIN_APP,
    TOKEN_KW_TOP,
    TOKEN_KW_VAR_REF,
    TOKEN_FALSE,
    TOKEN_TRUE,
    TOKEN_ID,
    TOKEN_STRING,
    TOKEN_FLOAT,
    TOKEN_INT,
    TOKEN_WHITESPACE,
    TOKEN_EOF
} ERTokenType;

typedef struct ERToken {
    char *value; 
    ERTokenType type; 
    int priority;
} ERToken;

char *shuntingYard(const char *expression);
char *ERpreProcess(char *expression);
char* expandIntervals(const char* er);
ERTokenType string_to_token_type(const char *str);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regular_expression.h"

#define MAX_TOKENS 100


#define BASE_PRIORITY 1000

int load_tokens_from_file(const char *filename, ERToken *out_ers) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir %s\n", filename);
        return 0;
    }

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;

        if (*ptr == '\n' || *ptr == '\r' || *ptr == ';' || *ptr == '\0') continue;

        char *arrow = strstr(ptr, "->");
        if (!arrow) continue;

        char *first_quote = strchr(ptr, '"');
        char *last_quote = NULL;
        
        char *temp = first_quote + 1;
        while (temp < arrow) {
            if (*temp == '"' && *(temp - 1) != '\\') {
                last_quote = temp;
            }
            temp++;
        }

        if (first_quote && last_quote && first_quote < last_quote) {
            *last_quote = '\0';
            char *regex_content = first_quote + 1;

            char token_name[128];
            if (sscanf(arrow + 2, " %s", token_name) == 1) {
                out_ers[count].value = strdup(regex_content);
                out_ers[count].type = string_to_token_type(token_name);
                
                out_ers[count].priority = 1000 - count; 
                count++;
            }
        }
    }

    fclose(file);
    return count;
}

const char* get_token_name(ERTokenType type) {
    switch(type) {
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_LBRACKET: return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET: return "TOKEN_RBRACKET";
        case TOKEN_DOT: return "TOKEN_DOT";
        
        case TOKEN_KW_MODULE_STAR: return "TOKEN_KW_MODULE_STAR";
        case TOKEN_KW_MODULE: return "TOKEN_KW_MODULE";
        case TOKEN_KW_BEGIN_FOR_SYNTAX: return "TOKEN_KW_BEGIN_FOR_SYNTAX";
        case TOKEN_KW_BEGIN0: return "TOKEN_KW_BEGIN0";
        case TOKEN_KW_BEGIN: return "TOKEN_KW_BEGIN";
        case TOKEN_KW_DEFINE_VALUES: return "TOKEN_KW_DEFINE_VALUES";
        case TOKEN_KW_DEFINE_SYNTAXES: return "TOKEN_KW_DEFINE_SYNTAXES";
        case TOKEN_KW_CASE_LAMBDA: return "TOKEN_KW_CASE_LAMBDA";
        case TOKEN_KW_IF: return "TOKEN_KW_IF";
        case TOKEN_KW_LETREC_VALUES: return "TOKEN_KW_LETREC_VALUES";
        case TOKEN_KW_LET_VALUES: return "TOKEN_KW_LET_VALUES";
        case TOKEN_KW_SET: return "TOKEN_KW_SET";
        case TOKEN_KW_QUOTE_SYNTAX: return "TOKEN_KW_QUOTE_SYNTAX";
        case TOKEN_KW_QUOTE: return "TOKEN_KW_QUOTE";
        case TOKEN_KW_WITH_CONT_MARK: return "TOKEN_KW_WITH_CONT_MARK";
        
        case TOKEN_KW_EXPRESSION: return "TOKEN_KW_EXPRESSION";
        case TOKEN_KW_PLAIN_MOD_BEGIN: return "TOKEN_KW_PLAIN_MOD_BEGIN";
        case TOKEN_KW_PROVIDE: return "TOKEN_KW_PROVIDE";
        case TOKEN_KW_DECLARE: return "TOKEN_KW_DECLARE";
        case TOKEN_KW_REQUIRE: return "TOKEN_KW_REQUIRE";
        case TOKEN_KW_PLAIN_LAMBDA: return "TOKEN_KW_PLAIN_LAMBDA";
        case TOKEN_KW_PLAIN_APP: return "TOKEN_KW_PLAIN_APP";
        case TOKEN_KW_TOP: return "TOKEN_KW_TOP";
        case TOKEN_KW_VAR_REF: return "TOKEN_KW_VAR_REF";
        case TOKEN_FALSE: return "TOKEN_FALSE";
        case TOKEN_TRUE: return "TOKEN_TRUE";
        case TOKEN_KW_LOCAL: return "TOKEN_KW_LOCAL";
        
        case TOKEN_ID: return "TOKEN_ID";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        
        case TOKEN_WHITESPACE: return "TOKEN_WHITESPACE";
        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_EMPTY: return "TOKEN_EMPTY / UNKNOWN";
        default: return "OTHER_TOKEN";
    }
}
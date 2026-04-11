#include "libs/regular_expression.h"

int main() {

    ERToken tokens[] = {
        {"if", TOKEN_IF, 1},
        {"(a|b|i|f)^", TOKEN_IDENTIFIER, 0},
        {"", TOKEN_EMPTY, 0}
    };

    
    return 0;
}
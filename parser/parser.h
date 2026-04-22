#pragma once

#include "../libs/tree.h"
#include "../libs/regular_expression.h"

typedef enum
{
    NODE_TOP_LEVEL = 1000,
    NODE_EXPRESSION,
    NODE_MODULE,
    NODE_MODULE_STAR,
    NODE_PLAIN_MODULE_BEGIN,
    NODE_BEGIN,
    NODE_BEGIN_FOR_SYNTAX,
    NODE_PROVIDE,
    NODE_DECLARE,
    NODE_DEFINE_VALUES,
    NODE_DEFINE_SYNTAXES,
    NODE_REQUIRE,
    NODE_PLAIN_LAMBDA,
    NODE_CASE_LAMBDA,
    NODE_IF,
    NODE_BEGIN0,
    NODE_LET_VALUES,
    NODE_LETREC_VALUES,
    NODE_SET,
    NODE_QUOTE,
    NODE_QUOTE_SYNTAX,
    NODE_WITH_CONT_MARK,
    NODE_PLAIN_APP,
    NODE_TOP,
    NODE_VAR_REF,
    NODE_ID,
    NODE_LITERAL,
    NODE_FORMALS,
    NODE_BINDING,
} AST_Node_Type;


TreeNode *parse(const char *filename);
ERTokenType *read_tokens(const char *filename, size_t *out_size);
#pragma once

#include "../libs/tree.h"
#include "../libs/regular_expression.h"
#include "../libs/scanner.h"
#include "../libs/regular_expression.h"

TreeNode *parse(TokenList *tokens, char **out_errors, size_t *out_size);
ERTokenType *read_tokens(const char *filename, size_t *out_size);
#pragma once

#include "regular_expression.h"

int load_tokens_from_file(const char *filename, ERToken *out_ers);
const char* get_token_name(ERTokenType type);
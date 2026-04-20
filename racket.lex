 "\(" -> TOKEN_LPAREN
 "\)" -> TOKEN_RPAREN
 "\[" -> TOKEN_LBRACKET
 "\]" -> TOKEN_RBRACKET
 "\." -> TOKEN_DOT

 "module\*" -> TOKEN_KW_MODULE_STAR
 "module" -> TOKEN_KW_MODULE
 "begin-for-syntax" -> TOKEN_KW_BEGIN_FOR_SYNTAX
 "begin0" -> TOKEN_KW_BEGIN0
 "begin" -> TOKEN_KW_BEGIN
 "define-values" -> TOKEN_KW_DEFINE_VALUES
 "define-syntaxes" -> TOKEN_KW_DEFINE_SYNTAXES
 "case-lambda" -> TOKEN_KW_CASE_LAMBDA
 "if" -> TOKEN_KW_IF
 "letrec-values" -> TOKEN_KW_LETREC_VALUES
 "let-values" -> TOKEN_KW_LET_VALUES
 "set!" -> TOKEN_KW_SET
 "quote-syntax" -> TOKEN_KW_QUOTE_SYNTAX
 "quote" -> TOKEN_KW_QUOTE
 "with-continuation-mark" -> TOKEN_KW_WITH_CONT_MARK

 "#%expression" -> TOKEN_KW_EXPRESSION
 "#%plain-module-begin" -> TOKEN_KW_PLAIN_MOD_BEGIN
 "#%provide" -> TOKEN_KW_PROVIDE
 "#%declare" -> TOKEN_KW_DECLARE
 "#%require" -> TOKEN_KW_REQUIRE
 "#%plain-lambda" -> TOKEN_KW_PLAIN_LAMBDA
 "#%plain-app" -> TOKEN_KW_PLAIN_APP
 "#%top" -> TOKEN_KW_TOP
 "#%variable-reference" -> TOKEN_KW_VAR_REF
 "#f" -> TOKEN_FALSE
 "#t" -> TOKEN_TRUE
 "#:local" -> TOKEN_KW_LOCAL

 "([0-9])([0-9])^\.([0-9])([0-9])^" -> TOKEN_FLOAT
 "([0-9])([0-9])^" -> TOKEN_INT

 "\"([a-z]|[A-Z]|[0-9]| |!|-|\?|_|\^|\.|,|:|;)^\"" -> TOKEN_STRING

 "([a-z]|[A-Z]|-|!|%|\&|\*|/|:|<|=|>|\?|~|_|\^|\+|#)([a-z]|[A-Z]|[0-9]|-|!|%|\&|\*|/|:|<|=|>|\?|~|_|\^|\+|#)^" -> TOKEN_ID

 "( |\t|\n|\r)( |\t|\n|\r)^" -> TOKEN_WHITESPACE
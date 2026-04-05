%config
IGNORE: [ \t\n\r]+
CASE_SENSITIVE: true

%def
DIGIT: [0-9]
LETTER: [a-zA-Z]
SYMBOL: [+-*=/<>?!:$%^&_~@.]
EPSILON: ""

%tokens

# Booleanos
"#true" | "#t" | "#T" -> TOKEN_BOOL_TRUE {uma anotação qualquer}
"#false" | "#f" | "#F" -> TOKEN_BOOL_FALSE

# Delimitadores
"(" -> TOKEN_LPAREN
")" -> TOKEN_RPAREN 
"[" -> TOKEN_LBRACKET
"]" -> TOKEN_RBRACKET 
"{" -> TOKEN_LBRACE 
"}" -> TOKEN_RBRACE 

TOKEN_NUMBER ->

("#%"|{EPSILON})({LETTER}|{DIGIT}|{SYMBOL})({LETTER}|{DIGIT}|{SYMBOL})* -> TOKEN_SYMBOL
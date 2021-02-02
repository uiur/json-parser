typedef enum {
  TOKEN_SYMBOL,
  TOKEN_WORD,
  TOKEN_NUMBER,
} TokenKind;

typedef struct Token {
  TokenKind kind;
  char* symbol;
  struct Token* next;
} Token;

typedef enum {
  NODE_PRIMARY_NUMBER,
  NODE_PRIMARY_STRING,
  NODE_PRIMARY_TRUE,
  NODE_PRIMARY_FALSE,
  NODE_PRIMARY_NULL,
  NODE_ARRAY,
  NODE_OBJECT,
  NODE_OBJECT_ENTRY,
} NODE_KIND;

typedef struct Node {
  NODE_KIND kind;
  int value;
  char* string;
  struct Node** children;
} Node;

Token* tokenize(char* string);
Node* parse(Token* token);
void print_node(Node* node);
void print_token(Token* token);

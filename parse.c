#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Node* alloc_node() {
  return (Node*)malloc(sizeof(Node));
}

Node* make_object(int size) {
  Node* node = alloc_node();
  node->kind = NODE_OBJECT;
  node->children = (Node**)calloc(size, sizeof(Node*));
  return node;
}

Node* make_object_entry(Node* key, Node* value) {
  Node* node = alloc_node();
  node->kind = NODE_OBJECT;
  node->children = (Node**)calloc(2, sizeof(Node*));
  node->children[0] = key;
  node->children[1] = value;
  return node;
}

Node* make_array(int size) {
  Node* node = alloc_node();
  node->kind = NODE_ARRAY;
  node->children = (Node**)calloc(size, sizeof(Node*));
  return node;
}

Node* make_number(int value) {
  Node* node = alloc_node();
  node->kind = NODE_PRIMARY_NUMBER;
  node->value = value;
  return node;
}

Node* make_string(char* string) {
  Node* node = alloc_node();
  node->kind = NODE_PRIMARY_STRING;
  node->string = string;
  return node;
}

void print_node(Node* node) {
  if (node->kind == NODE_OBJECT) {
    printf("{");

    int size;
    for (size = 0; node->children[size] != NULL; size++);

    for (int i = 0; node->children[i] != NULL; i++) {
      Node* entry = node->children[i];
      print_node(entry->children[0]);
      printf(": ");
      print_node(entry->children[1]);
      if (i != size - 1) {
        printf(", ");
      }
    }

    printf("}");
    return;
  }

  if (node->kind == NODE_ARRAY) {
    printf("[");
    int size;
    for (size = 0; node->children[size] != NULL; size++);

    for (int i = 0; node->children[i] != NULL; i++) {
      print_node(node->children[i]);
      if (i != size - 1) {
        printf(", ");
      }
    }
    printf("]");
    return;
  }

  if (node->kind == NODE_PRIMARY_NUMBER) {
    printf("%d", node->value);
  }

  if (node->kind == NODE_PRIMARY_STRING) {
    printf("\"%s\"", node->string);
  }

  if (node->kind == NODE_PRIMARY_TRUE) {
    printf("true");
  }

  if (node->kind == NODE_PRIMARY_FALSE) {
    printf("false");
  }

  if (node->kind == NODE_PRIMARY_NULL) {
    printf("null");
  }
}

char* symbols[] = { "{", "}", "[", "]", "\"", ":", ",", NULL };

void print_token(Token* token) {
  for (Token* current = token; current != NULL; current = current->next) {
    printf("`%s`", current->symbol);
    printf(" ");
  }
  printf("\n");
}

int is_whitespace(char c) {
  return c == ' ' || c == '\n';
}

int is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_number(char c) {
  return (c >= '0' && c <= '9');
}

Token* tokenize(char* string) {
  Token* head = malloc(sizeof(Token));
  Token* current = head;
  while (*string != '\0') {
    if (is_whitespace(*string)) {
      string++;
      continue;
    }

    int found = 0;
    for (int i = 0; symbols[i] != NULL; i++) {
      int symbol_length = strlen(symbols[i]);
      if (strncmp(symbols[i], string, symbol_length) == 0) {
        Token* token = malloc(sizeof(Token));
        token->kind = TOKEN_SYMBOL;
        token->symbol = symbols[i];
        current->next = token;
        current = token;
        string += symbol_length;
        found = 1;
        break;
      }
    }

    if (found) {
      continue;
    }

    // [a-zA-Z][a-zA-Z0-9]*
    if (is_alpha(*string)) {
      int size = 1;
      while (is_alpha(*(string + size)) || is_number(*(string + size))) {
        size++;
      }

      Token* token = malloc(sizeof(Token));
      token->kind = TOKEN_WORD;
      token->symbol = malloc(sizeof(char) * (size + 1));
      strncpy(token->symbol, string, size);

      current->next = token;
      current = token;
      string += size;
      continue;
    }

    if (*string == '-' || is_number(*string)) {
      int size = 1;
      while (is_number(*(string + size))) {
        size++;
      }

      Token* token = malloc(sizeof(Token));
      token->kind = TOKEN_NUMBER;
      token->symbol = malloc(sizeof(char) * (size + 1));
      strncpy(token->symbol, string, size);

      current->next = token;
      current = token;
      string += size;
      continue;
    }

    print_token(head->next);
    printf("unexpected token: `%s`\n", string);
    abort();
  }

  return head->next;
}

Node* expression(Token **token_ref);

void next(Token** token_ref) {
  *token_ref = (*token_ref)->next;
}

void consume(Token** token_ref, char* symbol) {
  if (*token_ref == NULL) {
    printf("expected `%s`, but got %s\n", symbol, "NULL");
    abort();
  }

  if (*token_ref == NULL || strcmp((*token_ref)->symbol, symbol) != 0) {
    printf("expect `%s`, but got `%s`\n", symbol, (*token_ref)->symbol);
    abort();
  }

  next(token_ref);
}

int accept(Token** token_ref, char* symbol) {
  if (*token_ref == NULL) return 0;
  if (strcmp((*token_ref)->symbol, symbol) == 0) {
    next(token_ref);
    return 1;
  }

  return 0;
}

Node* string(Token** token_ref) {
  if (accept(token_ref, "\"")) {
    Node* node = make_string((*token_ref)->symbol);
    next(token_ref);

    consume(token_ref, "\"");
    return node;
  }

  return NULL;
}

Node* boolean(Token** token_ref) {
  if (accept(token_ref, "true")) {
    Node* node = alloc_node();
    node->kind = NODE_PRIMARY_TRUE;
    return node;
  }

  if (accept(token_ref, "false")) {
    Node* node = alloc_node();
    node->kind = NODE_PRIMARY_FALSE;
    return node;
  }

  return NULL;
}

Node* null(Token** token_ref) {
  if (accept(token_ref, "null")) {
    Node* node = alloc_node();
    node->kind = NODE_PRIMARY_NULL;
    return node;
  }

  return NULL;
}

Node* object_entry(Token** token_ref) {
  Node* key = string(token_ref);
  if (key == NULL) {
    return NULL;
  }

  consume(token_ref, ":");
  Node* value = expression(token_ref);

  return make_object_entry(key, value);
}

void expect_token_kind(Token *token, TokenKind kind) {
  if (token->kind == kind) {
  }
}

Node* number(Token** token_ref) {
  if ((*token_ref)->kind == TOKEN_NUMBER) {
    int n = (int)atoi((*token_ref)->symbol);
    next(token_ref);
    return make_number(n);
  }

  return NULL;
}

void push_child(Node* node, Node* child) {
  int i = 0;
  while (node->children[i] != NULL) i++;

  node->children = (Node**)realloc(node->children, sizeof(Node) * (i + 1));
  node->children[i] = child;
}

Node* object_expression(Token **token_ref) {
  if (accept(token_ref, "{")) {
    Node* object = make_object(0);
    while (1) {
      Node* entry = object_entry(token_ref);
      if (entry == NULL) break;
      push_child(object, entry);

      if (!accept(token_ref, ",")) break;
    }

    consume(token_ref, "}");

    return object;
  }

  return NULL;
}


Node* array_expression(Token** token_ref) {
  if (accept(token_ref, "[")) {
    Node* array_node = make_array(0);
    while (1) {
      Node* value = expression(token_ref);
      if (value == NULL) break;
      push_child(array_node, value);

      if (!accept(token_ref, ",")) break;
    }

    consume(token_ref, "]");

    return array_node;
  }

  return NULL;
}


Node* primary(Token** token_ref) {
  Node* node;

  node = number(token_ref);
  if (node != NULL) return node;

  node = string(token_ref);
  if (node != NULL) return node;

  node = boolean(token_ref);
  if (node != NULL) return node;

  node = null(token_ref);
  if (node != NULL) return node;

  return NULL;
}

Node* expression(Token** token_ref) {
  Node* node;
  node = object_expression(token_ref);
  if (node != NULL) {
    return node;
  }

  node = array_expression(token_ref);
  if (node != NULL) {
    return node;
  }

  node = primary(token_ref);
  if (node != NULL) {
    return node;
  }

  return NULL;
}

Node* parse(Token* token) {
  return expression(&token);
}

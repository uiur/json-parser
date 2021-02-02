#include "parse.h"

#include <stdio.h>

char buf[10000];
int main(int argc, char **argv) {
  char c;
  int i;
  while ((c = getchar()) != EOF) {
    buf[i++] = c;
  }
  int size = i;
  buf[size] = '\0';

  Token* token = tokenize(buf);
  Node* node = parse(token);
  print_node(node);

  return 0;
}

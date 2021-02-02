#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char* data[] = {
    "0",
    "1234",
    "-12",
    "\"bar123\"",
    "null",
    "true",
    "false",
    // object
    "{}",
    "{\"foo\": 1,\"bar\": 2}",
    "{\"foo\": 1,\"bar\": { \"foo2\": 2 }}",
    "{\"fooBar\": {}}",

    // array
    "[]",
    "[ 1, 2, \"three\" ]",
    "[ { \"foo\": 1 } ]",
    "[ {} ]",
    NULL
  };

  for (int i = 0; data[i] != NULL; i++) {
    char* input = data[i];
    printf("input:\t%s\n", input);
    Token* token = tokenize(input);
    printf("token:\t");
    print_token(token);

    Node* node = parse(token);
    printf("parse:\t");
    print_node(node);
    printf("\n\n");
  }

  return 0;
}

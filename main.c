#include "parse.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

void json_value_print(JsonValue *value) {
  if (value == NULL) {
    fprintf(stderr, "unexpected null\n");
    abort();
  }

  if (value->type == JSON_VALUE_STRING) {
    JsonString *s = (JsonString*)value;
    printf("%s", s->value);
  } else if (value->type == JSON_VALUE_NUMBER) {
    printf("%d", ((JsonNumber*)value)->value);
  }
}

char buf[10000];
int main(int argc, char **argv) {
  JsonObject *jsonObject = json_object_new();

  JsonString *jsonString = malloc(sizeof(JsonString));
  jsonString->type = JSON_VALUE_STRING;
  jsonString->value = "foo";

  JsonNumber *jsonNumber = malloc(sizeof(JsonNumber));
  jsonNumber->type = JSON_VALUE_NUMBER;
  jsonNumber->value = 10;

  json_object_write(jsonObject, jsonString, (JsonValue *)jsonNumber);
  JsonValue *value = json_object_read(jsonObject, jsonString);
  json_value_print(value);
  printf("\n");

  return 0;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <selector>\n", argv[0]);
    return 1;
  }

  char* selector = argv[1];
  printf("%s\n", selector);

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

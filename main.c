#include "parse.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

void json_value_filter(JsonValue *value, char *selector) {
  if (value->type == JSON_VALUE_ARRAY) {
    int length = json_array_length((JsonArray*)value);
    for (int i = 0; i < length; i++) {
      JsonValue *item = json_array_read((JsonArray*)value, i);
      json_value_filter(item, selector);
    }
    return;
  }

  if (value->type == JSON_VALUE_OBJECT) {
    JsonValue *object_value = json_object_read((JsonObject*)value, json_string_new(selector));
    if (object_value != NULL) {
      json_value_print(object_value);
    }

    return;
  }
}

char buf[10000];
int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <selector>\n", argv[0]);
    return 1;
  }

  char* selector = argv[1];

  char c;
  int i;
  while ((c = getchar()) != EOF) {
    buf[i++] = c;
  }
  int size = i;
  buf[size] = '\0';

  Token *token = tokenize(buf);
  Node *node = parse(token);

  JsonValue *value = evaluate(node);
  // json_value_print(value);
  printf("\n");
  json_value_filter(value, selector);
  printf("\n");

  return 0;
}

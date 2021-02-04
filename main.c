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

  for (int i = 0; i < 20; i++) {
    char *c = calloc(2, sizeof(char));
    c[0] = 'a' + i;
    printf("%s: %d\n", c, i);

    json_object_write(jsonObject, json_string_new(c), (JsonValue*)json_number_new(i));
  }

  JsonValue *value = json_object_read(jsonObject, json_string_new("c"));
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

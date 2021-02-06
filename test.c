#include "parse.h"
#include "value.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
  "{\"fooBar\": { \"bar\": true }}",

  // array
  "[]",
  "[ 1, 2, \"three\" ]",
  "[ { \"foo\": 1 } ]",
  "[ {} ]",
  NULL
};

void test_parse() {
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
}

void test_value() {
  json_value_print((JsonValue*)json_string_new("foo"));
  printf("\n");
  json_value_print((JsonValue*)json_number_new(42));
  printf("\n");

  JsonObject* object = json_object_new();
  json_value_print((JsonValue*)object);
  printf("\n");

  {
    JsonObject* object = json_object_new();
    json_object_write(object, json_string_new("foo"), (JsonValue*)json_number_new(12));
    json_object_write(object, json_string_new("bar"), (JsonValue*)json_number_new(34));
    json_value_print((JsonValue*)object);
    printf("\n");
  }

  {
    JsonArray* array = json_array_new();
    json_array_write(array, 0, (JsonValue*)json_number_new(1));
    json_array_write(array, 1, (JsonValue*)json_number_new(2));
    json_value_print((JsonValue*)array);
    printf("\n");
  }
}

void test_eval() {
  for (int i = 0; data[i] != NULL; i++) {
    char* input = data[i];

    printf("input: %s\n", input);

    JsonValue* value = eval(input);
    if (value == NULL) {
      printf("got: NULL\n");
      abort();
    }

    json_value_print(value);
    printf("\n\n");
  }
}

int main(int argc, char **argv) {
  test_parse();
  test_value();
  test_eval();

  return 0;
}

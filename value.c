#include "parse.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define JSON_OBJECT_RESIZE_THRESHOLD 0.75

JsonNumber* json_number_new(int num) {
  JsonNumber *jsonNumber = malloc(sizeof(JsonNumber));
  jsonNumber->type = JSON_VALUE_NUMBER;
  jsonNumber->value = num;
  return jsonNumber;
}

JsonString* json_string_new(char* str) {
  JsonString *jsonString = malloc(sizeof(JsonString));
  jsonString->type = JSON_VALUE_STRING;
  jsonString->value = str;

  return jsonString;
}


int json_object_hash(int size, char* s) {
  int h = 0;
  for (char* p = s; *p != '\0'; p++) {
    h = (h * 31 + *p) % size;
  }

  return h;
}

JsonObject* json_object_new() {
  JsonObject *object = malloc(sizeof(JsonObject));
  object->type = JSON_VALUE_OBJECT;
  object->size = 10;
  object->used = 0;

  object->keys = calloc(object->size, sizeof(JsonString*));

  object->containers = calloc(object->size, sizeof(JsonObjectEntryContainer*));
  for (int i = 0; i < object->size; i++) {
    object->containers[i] = malloc(sizeof(JsonObjectEntryContainer));
  }

  return object;
}

void json_object_resize(JsonObject *object, int new_size) {
  JsonObjectEntryContainer **new_containers = calloc(new_size, sizeof(JsonObjectEntryContainer*));

  for (int i = 0; i < new_size; i++) {
    new_containers[i] = malloc(sizeof(JsonObjectEntryContainer));
  }

  for (int i = 0; i < object->size; i++) {
    JsonObjectEntryContainer *container = object->containers[i];

    while (container != NULL && container->value != NULL) {
      JsonObjectEntry *entry = container->value;

      int new_hash = json_object_hash(new_size, entry->key->value);
      new_containers[new_hash] = container;
      JsonObjectEntryContainer *next = container->next;
      container->next = NULL;
      container = next;
    }
  }

  free(object->containers);
  object->size = new_size;
  object->containers = new_containers;
}

void json_object_resize_if_needed(JsonObject *object) {
  if ((object->used) >= (object->size) * JSON_OBJECT_RESIZE_THRESHOLD) {
    json_object_resize(object, pow(object->size, 2));
  }
}

void json_object_write(JsonObject *object, JsonString *key, JsonValue *value)  {
  int hash_value = json_object_hash(object->size, key->value);

  // find matched key
  JsonObjectEntryContainer *container = object->containers[hash_value];
  for (; container->next != NULL;  container = container->next) {
    JsonObjectEntry *entry = container->value;
    if (strcmp(entry->key->value, key->value) == 0) {
      entry->value = value;
      return;
    }
  }

  JsonObjectEntry *entry = malloc(sizeof(JsonObjectEntry));
  entry->key = key;
  entry->value = value;

  // insert entry
  json_object_resize_if_needed(object);

  container->value = entry;
  container->next = malloc(sizeof(JsonObjectEntryContainer));

  object->keys[object->used] = key;
  object->used += 1;
}

JsonValue* json_object_read(JsonObject *object, JsonString *key) {
  int hash_value = json_object_hash(object->size, key->value);

  JsonObjectEntryContainer *container = object->containers[hash_value];
  for (; container->value != NULL; container = container->next) {
    JsonObjectEntry *entry = container->value;
    if (strcmp(entry->key->value, key->value) == 0) {
      return entry->value;
    }
  }

  return NULL;
}

JsonArray* json_array_new() {
  JsonArray *array = malloc(sizeof(JsonArray));
  array->type = JSON_VALUE_ARRAY;
  array->object = json_object_new();
  return array;
}

void json_array_write(JsonArray *array, int index, JsonValue *value) {
  char *c = calloc(2, sizeof(char));
  c[0] = '0' + index;
  json_object_write(array->object, json_string_new(c), value);
}

JsonValue* json_array_read(JsonArray *array, int index) {
  char *c = calloc(2, sizeof(char));
  c[0] = '0' + index;
  return json_object_read(array->object, json_string_new(c));
}

int json_array_length(JsonArray *array) {
  return array->object->used;
}

char* json_stringify(JsonValue *value) {
  if (value->type == JSON_VALUE_STRING) {
    JsonString *s = (JsonString*)value;
    char *buf = calloc(strlen(s->value) + 10, sizeof(char));
    sprintf(buf, "\"%s\"", s->value);
    return buf;
  } else if (value->type == JSON_VALUE_NUMBER) {
    char *buf = calloc(1000, sizeof(char));
    sprintf(buf, "%d", ((JsonNumber*)value)->value);
    return buf;
  } else if (value->type == JSON_VALUE_OBJECT) {
    JsonObject* object = (JsonObject*)value;

    // todo: dynamic allocation
    char inner[10000] = "";

    for (int i = 0; i < object->used; i++) {
      if (object->keys[i] == NULL) continue;

      JsonString *key = object->keys[i];
      JsonValue *value = json_object_read(object, key);
      if (value == NULL) continue;

      char *key_str = json_stringify((JsonValue*)key);
      char *value_str = json_stringify(value);

      strcat(inner, key_str);
      strcat(inner, ": ");
      strcat(inner, value_str);

      if (i < object->used - 1) {
        strcat(inner, ", ");
      }
    }

    char *buf = calloc(strlen(inner) + 10, sizeof(char));
    sprintf(buf, "{%s}", inner);
    return buf;
  } else if (value->type == JSON_VALUE_ARRAY) {
    JsonArray* array = (JsonArray*)value;
    JsonObject* object = array->object;

    char inner[10000] = "";

    for (int i = 0; i < object->used; i++) {
      if (object->keys[i] == NULL) continue;

      JsonString *key = object->keys[i];
      JsonValue *value = json_object_read(object, key);
      if (value == NULL) continue;

      char *value_str = json_stringify(value);

      strcat(inner, value_str);
      if (i < object->used - 1) {
        strcat(inner, ", ");
      }
    }

    char *buf = calloc(strlen(inner) + 10, sizeof(char));
    sprintf(buf, "[%s]", inner);
    return buf;

    return json_stringify((JsonValue*)array->object);
  } else {
    fprintf(stderr, "unexpected type: %i\n", value->type);
    abort();
  }
}

JsonValue* evaluate(Node *node) {
  if (node->kind == NODE_PRIMARY_NUMBER) {
    return (JsonValue*) json_number_new(node->value);
  }

  if (node->kind == NODE_PRIMARY_STRING) {
    return (JsonValue*)json_string_new(node->string);
  }

  if (node->kind == NODE_OBJECT) {
    JsonObject *json_object = json_object_new();

    for (int i = 0; node->children[i] != NULL; i++){
      Node *entry_node = node->children[i];
      Node *key_node = entry_node->children[0];
      Node *value_node = entry_node->children[1];

      JsonValue *key = evaluate(key_node);
      JsonValue *value = evaluate(value_node);

      json_object_write(json_object, (JsonString*)key, value);
    }

    return (JsonValue*)json_object;
  }

  if (node->kind == NODE_ARRAY) {
    JsonArray *json_array = json_array_new();

    for (int i = 0; node->children[i] != NULL; i++) {
      Node *value_node = node->children[i];
      JsonValue *value = evaluate(value_node);

      json_array_write(json_array, i, value);
    }

    return (JsonValue*)json_array;
  }

  fprintf(stderr, "unexpected node: %d", node->kind);
  abort();

  // NODE_PRIMARY_TRUE,
  // NODE_PRIMARY_FALSE,
  // NODE_PRIMARY_NULL,
}

void json_value_print(JsonValue *value) {
  if (value == NULL) {
    fprintf(stderr, "unexpected null\n");
    abort();
  }

  char *json_str = json_stringify(value);
  printf("%s", json_str);
}

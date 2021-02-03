#include "parse.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JsonObject* json_object_new() {
  JsonObject *object = malloc(sizeof(JsonObject));
  // todo: resize
  object->type = JSON_VALUE_OBJECT;
  object->size = 1000;
  object->containers = calloc(object->size, sizeof(JsonObjectEntryContainer*));
  for (int i = 0; i < object->size; i++) {
    object->containers[i] = malloc(sizeof(JsonObjectEntryContainer));
  }

  return object;
}

int json_object_hash(int size, char* s) {
  int h = 0;
  for (char* p = s; *p != '\0'; p++) {
    h = (h * 31 + *p) % size;
  }

  return h;
}

void json_object_write(JsonObject *object, JsonString *key, JsonValue *value)  {
  int hash_value = json_object_hash(object->size, key->value);

  JsonObjectEntry *entry = malloc(sizeof(JsonObjectEntry));
  entry->key = key;
  entry->value = value;

  // override
  // todo: chain
  JsonObjectEntryContainer *container = object->containers[hash_value];
  if (container->value != NULL) {
    fprintf(stderr, "collide!\n");
    abort();
  }

  container->value = entry;
}

JsonValue* json_object_read(JsonObject *object, JsonString *key) {
  int hash_value = json_object_hash(object->size, key->value);

  JsonObjectEntryContainer *container = object->containers[hash_value];
  if (container->value == NULL) {
    return NULL;
  }

  JsonObjectEntry *entry = container->value;
  if (strcmp(entry->key->value, key->value) == 0) {
    return entry->value;
  }

  // todo: handle

  return NULL;
}

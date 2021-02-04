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

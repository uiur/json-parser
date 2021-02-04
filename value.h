enum JsonValueType {
  JSON_VALUE_NUMBER,
  JSON_VALUE_STRING,
  JSON_VALUE_NULL,
  JSON_VALUE_OBJECT,
  JSON_VALUE_ARRAY
};

typedef struct JsonString {
  enum JsonValueType type;
  char* value;
} JsonString;

typedef struct JsonNumber {
  enum JsonValueType type;
  int value;
} JsonNumber;

typedef struct JsonValue {
  enum JsonValueType type;
} JsonValue;

typedef struct JsonObjectEntry {
  JsonString *key;
  JsonValue *value;
} JsonObjectEntry;

typedef struct JsonObjectEntryContainer {
  struct JsonObjectEntry *value;
  struct JsonObjectEntryContainer *next;
} JsonObjectEntryContainer;

typedef struct JsonObject {
  enum JsonValueType type;
  int size;
  int used;
  struct JsonString **keys;
  struct JsonObjectEntryContainer **containers;
} JsonObject;

JsonNumber* json_number_new(int n);
JsonString* json_string_new(char *str);

JsonObject* json_object_new();
void json_object_write(JsonObject *object, JsonString *key, JsonValue *value);
JsonValue* json_object_read(JsonObject *object, JsonString *key);

void json_value_print(JsonValue *value);

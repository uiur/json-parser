enum JsonValueType {
  JSON_VALUE_NUMBER,
  JSON_VALUE_STRING,
  JSON_VALUE_NULL,
  JSON_VALUE_BOOLEAN,
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

typedef struct JsonNull {
  enum JsonValueType type;
} JsonNull;

typedef struct JsonBoolean {
  enum JsonValueType type;
  int value;
} JsonBoolean;

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

typedef struct JsonArray {
  enum JsonValueType type;
  struct JsonObject *object;
} JsonArray;

JsonNumber* json_number_new(int n);
JsonString* json_string_new(char *str);
JsonNull* json_null_new();
JsonBoolean* json_boolean_new(int value);

JsonObject* json_object_new();
void json_object_write(JsonObject *object, JsonString *key, JsonValue *value);
JsonValue* json_object_read(JsonObject *object, JsonString *key);

JsonArray* json_array_new();
void json_array_write(JsonArray *object, int index, JsonValue *value);
JsonValue* json_array_read(JsonArray *object, int index);
int json_array_length(JsonArray *object);

JsonValue* evaluate(Node *node);

JsonValue* eval(char *source);
void json_value_print(JsonValue *value);

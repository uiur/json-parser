#include <stdio.h>
#include <stdlib.h>

typedef enum {
  NODE_PRIMARY_NUMBER,
  NODE_PRIMARY_STRING,
  NODE_ARRAY,
  NODE_OBJECT,
  NODE_OBJECT_ENTRY,
} NODE_KIND;

typedef struct Node {
  NODE_KIND kind;
  int value;
  char* string;
  struct Node** children;
} Node;

Node* alloc_node() {
  return (Node*)malloc(sizeof(Node));
}

Node* make_object(int size) {
  Node* node = alloc_node();
  node->kind = NODE_OBJECT;
  node->children = (Node**)malloc(sizeof(Node*) * (size + 1));
  return node;
}

Node* make_object_entry(Node* key, Node* value) {
  Node* node = alloc_node();
  node->kind = NODE_OBJECT;
  node->children = (Node**)malloc(sizeof(Node*) * 2);
  node->children[0] = key;
  node->children[1] = value;
  return node;
}

Node* make_array(int size) {
  Node* node = alloc_node();
  node->kind = NODE_ARRAY;
  node->children = (Node**)malloc(sizeof(Node*) * (size + 1));
  return node;
}

Node* make_number(int value) {
  Node* node = alloc_node();
  node->kind = NODE_PRIMARY_NUMBER;
  node->value = value;
  return node;
}

Node* make_string(char* string) {
  Node* node = alloc_node();
  node->kind = NODE_PRIMARY_STRING;
  node->string = string;
  return node;
}

void print_node(Node* node) {
  if (node->kind == NODE_OBJECT) {
    printf("{ ");

    int size;
    for (size = 0; node->children[size] != NULL; size++);

    for (int i = 0; node->children[i] != NULL; i++) {
      Node* entry = node->children[i];
      print_node(entry->children[0]);
      printf(": ");
      print_node(entry->children[1]);
      if (i != size - 1) {
        printf(", ");
      }
    }

    printf(" }");
    return;
  }

  if (node->kind == NODE_ARRAY) {
    printf("[");
    int size;
    for (size = 0; node->children[size] != NULL; size++);

    for (int i = 0; node->children[i] != NULL; i++) {
      print_node(node->children[i]);
      if (i != size - 1) {
        printf(", ");
      }
    }
    printf("]");
    return;
  }

  if (node->kind == NODE_PRIMARY_NUMBER) {
    printf("%d", node->value);
  }

  if (node->kind == NODE_PRIMARY_STRING) {
    printf("\"%s\"", node->string);
  }
}

int main(int argc, char **argv) {
  Node* array = make_array(2);
  array->children[0] = make_number(1);
  array->children[1] = make_number(2);

  print_node(array);
  printf("\n");

  Node* object = make_object(2);
  object->children[0] = make_object_entry(make_string("foo"), make_number(2));
  object->children[1] = make_object_entry(make_string("bar"), make_number(3));
  print_node(object);
  printf("\n");


  // 0, 1, null, true, false, [], 'foobar', {}
  // { "foo": 123, "bar": 456 }
  // [0, 1, 2]
  // Node { kind, size, args }
  // Node<Number> { double value }
  // Node<Boolean> { int value = 0 | 1 }
  // Node<String> { char* value = "foobar" }
  // Node<Null> {  }
  // Node<Array> { Node** list = [Node*, Node* ...] }
  // Node<Object> { Node* entries = [Node<Entry>*, Node<Entry>*, Node<Entry>* ...] }
  // Node<Entry> { Node* key = Node<String> Node* value = Node* }

  return 0;
}

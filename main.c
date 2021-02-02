#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// expression := object_expression | array_expression | primary
// object_expression := '{' (object_entry (',' object_entry)*)? '}'
// object_entry := string ':' expression
// array_expression := '[' (expression (',' expression)*)? ']'
// primary := number | string | boolean | null
// string := '"' [a-zA-Z][a-zA-Z0-9]* '"'
// number := [\-]?[0-9]+
// null := 'null'

int main(int argc, char **argv) {
  char* s[] = {
    "{\"foo\": 1,\"bar\": 2}",
    "{\"foo\": 1,\"bar\": { \"foo2\": 2 }}",
    "[ 1, 2, \"three\" ]",

    NULL
  };

  for (int i = 0; s[i] != NULL; i++) {
    Token* token = tokenize(s[i]);
    print_token(token);

    Node* node = parse(token);
    print_node(node);
    printf("\n\n");
  }

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

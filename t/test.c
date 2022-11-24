#include "../deps/tap.c/tap.h"

#include "../src/array.h"

#include <stdlib.h>

void test_array_init() {
  Array *array;
  lives_ok({
    array = array_init();
  }, "initializes array");

  cmp_ok((int)array->len, "==", 0, "initializes the array's length to zero");
  is(array->state, "", "initializes state to empty");
}

void test_array_push() {
  Array *array = array_init();

  char values[] = { 'x', 'y', 'z' };
  for (int i = 0; i < 3; i++) {
    char el = values[i];

    cmp_ok(array_push(array, &el), "==", true, "returns true when successful");
    cmp_ok((int)array->len, "==", i + 1, "increases the array's length by one");
    cmp_ok(array->state[i], "==", &el, "adds the element to the end of the array");
  }
}

void test_array_pop() {
  Array *array = array_init();

  char values[] = { 'x', 'y', 'z' };
  int i = 0;
  for (; i < 3; i++) {
    array_push(array, &values[i]);
  }
  for (; i > 0; i--) {
    cmp_ok(values[i - 1], "==", *((char*)array_pop(array)), "removes the element from the end of the array");
    cmp_ok((int)array->len, "==", i - 1, "decreases the array's length by one");
  }
}

int main() {
	plan(0);

  test_array_init();
  test_array_push();
  test_array_pop();

  done_testing();
}

#include "array.h"

#include <stdlib.h>

#include "tap.c/tap.h"

Array *make_test_array() {
  Array *array = array_init();

  char values[] = {'x', 'y', 'z', '1', '2', '3'};
  for (int i = 0; i < 6; i++) {
    array_push(array, (void *)values[i]);
  }

  return array;
}

void test_array_init() {
  Array *array;
  lives_ok({ array = array_init(); }, "initializes array");

  cmp_ok((int)array->len, "==", 0, "initializes the array's length to zero");
  is(array->state, "", "initializes state to empty");
}

bool *will_include(void *el) { return el == 'z'; }

bool *wont_include(void *el) { return el == 'n'; }

void test_array_includes() {
  Array *array = make_test_array();

  cmp_ok(array_includes(array, will_include), "==", true,
         "returns true when it includes the element");
  cmp_ok(array_includes(array, wont_include), "==", false,
         "returns false when it does not include the element");
}

void test_array_push() {
  Array *array = array_init();

  char values[] = {'x', 'y', 'z'};
  for (int i = 0; i < 3; i++) {
    void *el = values[i];

    cmp_ok(array_push(array, el), "==", true, "returns true when successful");
    cmp_ok((int)array->len, "==", i + 1, "increases the array's length by one");
    cmp_ok(array->state[i], "==", el,
           "adds the element to the end of the array");
  }
}

void test_array_pop() {
  Array *array = array_init();

  char values[] = {'x', 'y', 'z'};
  int i = 0;
  for (; i < 3; i++) {
    array_push(array, values[i]);
  }
  for (; i > 0; i--) {
    cmp_ok(array_pop(array), "==", values[i - 1],
           "removes the element from the end of the array");
    cmp_ok((int)array->len, "==", i - 1, "decreases the array's length by one");
  }
}

void test_array_pop_empty() {
  Array *array = array_init();

  char values[] = {'x', 'y', 'z'};
  int i = 0;
  for (; i < 3; i++) {
    array_push(array, values[i]);
  }
  for (; i > 0; i--) {
    array_pop(array);
  }

  is(array_pop(array), NULL, "returns NULL from an empty array");
}

void test_array_slice() {
  Array *array = make_test_array();
  Array *sliced = array_slice(array, 1, 4);

  cmp_ok((int)sliced->len, "==", 3, "slices the range of elements inclusively");

  for (int i = 0; i < 3; i++) {
    cmp_ok(sliced->state[i], "==", array->state[i + 1],
           "contains the sliced elements");
  }
}

#define MAPPER_INC 10
void *mapper(void *el, int index, Array *array) { return el + MAPPER_INC; }

void test_array_map() {
  Array *array = make_test_array();

  Array *transformed = array_map(array, mapper);
  cmp_ok(transformed->len, "==", array->len,
         "has the same length as the input array");

  for (int i = 0; i < 3; i++) {
    cmp_ok(transformed->state[i], "==", array->state[i] + MAPPER_INC,
           "applies the function to each element");
  }
}

bool *filter(void *el, int index, Array *array) { return el > (int)'A'; }
void test_array_filter() {
  Array *array = make_test_array();

  Array *transformed = array_filter(array, filter);
  cmp_ok(transformed->len, "==", 3, "has a length of filtered elements only");
  for (int i = 0; i < array->len; i++) {
    void *el = array->state[i];

    if (el > 'A') {
      cmp_ok(transformed->state[i], "==", el,
             "includes only elements that match the filter condition");
    }
  }
}

int main() {
  plan(33);

  test_array_init();
  test_array_includes();
  test_array_push();
  test_array_pop();
  test_array_pop_empty();
  test_array_slice();
  test_array_map();
  test_array_filter();

  done_testing();
}

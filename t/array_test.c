#include <stdlib.h>

#include "libutil.h"
#include "tap.c/tap.h"

array_t *make_test_array(void) {
  array_t *array = array_init();

  char values[] = {'x', 'y', 'z', '1', '2', '3'};
  for (unsigned int i = 0; i < 6; i++) {
    array_push(array, (void *)values[i]);
  }

  return array;
}

void test_array_init(void) {
  array_t *array;

  lives_ok({ array = array_init(); }, "initializes array");
  cmp_ok(array_size(array), "==", 0, "initializes the array's length to zero");

  array_free(array);
}

void test_array_size(void) {
  array_t *array = array_init();

  cmp_ok(array_size(array), "==", 0, "retrieves initial size");
  array_push(array, "test");
  cmp_ok(array_size(array), "==", 1, "tracks size pointer");

  array_free(array);
}

void test_array_get(void) {
  array_t *array = array_init();
  is(array_get(array, 0), NULL, "returns NULL if empty");
  is(array_get(array, 4), NULL, "index larger than len returns NULL");
  is(array_get(array, -1), NULL, "-1 index returns NULL");
  is(array_get(array, -4), NULL, "negative index larger than len returns NULL");

  array_push(array, 10);
  array_push(array, 11);
  array_push(array, 12);

  is(array_get(array, 4), NULL, "index larger than len returns NULL");
  cmp_ok(array_get(array, -1), "==", 12, "-1 index returns last element");
  cmp_ok(array_get(array, 1), "==", 11, "index returns correct element");
  cmp_ok(array_get(array, -2), "==", 11, "index returns correct element");

  array_free(array);
}

void test_array_includes(void) {
  array_t *array = make_test_array();

  cmp_ok(array_includes(array, int_comparator, 'z'), "==", true,
         "returns true when it includes the element");
  cmp_ok(array_includes(array, int_comparator, 'n'), "==", false,
         "returns false when it does not include the element");

  array_free(array);
}

void test_array_find(void) {
  array_t *array = make_test_array();
  cmp_ok((int)array_find(array, int_comparator, 'z'), "==", 2,
         "returns index when it finds the element");
  cmp_ok(array_find(array, int_comparator, 'n'), "==", -1,
         "returns -1 when it does not find the element");

  array_free(array);

  array_t *strarr = array_collect("92Q", "dew");
  cmp_ok((int)array_find(array, str_comparator, "dew"), "==", 1,
         "returns index when it finds the string element");
}

void test_array_push(void) {
  __array_t *array = (__array_t *)array_init();

  char values[] = {'x', 'y', 'z'};
  for (unsigned int i = 0; i < 3; i++) {
    void *el = (void *)values[i];

    cmp_ok(array_push((array_t *)array, el), "==", true,
           "returns true when successful");
    cmp_ok(array->len, "==", i + 1, "increases the array's length by one");
    cmp_ok((int)array->state[i], "==", (int)el,
           "adds the element to the end of the array");
  }

  array_free(array);
}

void test_array_pop(void) {
  array_t *array = array_init();

  char values[] = {'x', 'y', 'z'};
  unsigned int i = 0;
  for (; i < 3; i++) {
    array_push(array, (void *)values[i]);
  }
  for (; i > 0; i--) {
    cmp_ok((int)array_pop(array), "==", values[i - 1],
           "removes the element from the end of the array");
    cmp_ok((int)((__array_t *)array)->len, "==", i - 1,
           "decreases the array's length by one");
  }

  array_free(array);
}

void test_array_pop_empty(void) {
  array_t *array = array_init();

  char values[] = {'x', 'y', 'z'};
  unsigned int i = 0;
  for (; i < 3; i++) {
    array_push(array, (void *)values[i]);
  }
  for (; i > 0; i--) {
    array_pop(array);
  }

  is(array_pop(array), NULL, "returns NULL from an empty array");

  array_free(array);
}

void test_array_shift(void) {
  array_t *array = array_init();
  __array_t *internal = (__array_t *)array;

  char values[] = {'x', 'y', 'z'};
  for (unsigned int i = 0; i < 3; i++) {
    array_push(array, (void *)values[i]);
  }

  cmp_ok((int)array_shift(array), "==", 'x');
  cmp_ok(internal->len, "==", 2);
  cmp_ok((int)internal->state[0], "==", 'y');
  cmp_ok((int)internal->state[1], "==", 'z');

  cmp_ok((int)array_shift(array), "==", 'y');
  cmp_ok((int)internal->len, "==", 1);
  cmp_ok((int)internal->state[0], "==", 'z');

  cmp_ok((int)array_shift(array), "==", 'z');
  cmp_ok(internal->len, "==", 0);

  array_free(array);
}

void test_array_shift_empty(void) {
  array_t *array = array_init();

  is(array_shift(array), NULL, "shift on an empty array returns NULL");

  array_free(array);
}

void test_array_slice(void) {
  array_t *array = make_test_array();
  __array_t *sliced = (__array_t *)array_slice(array, 1, 4);

  cmp_ok(sliced->len, "==", 3, "slices the range of elements inclusively");

  for (unsigned int i = 0; i < 3; i++) {
    cmp_ok((int)sliced->state[i], "==", (int)((__array_t *)array)->state[i + 1],
           "contains the sliced elements");
  }

  array_free(array);
  array_free(sliced);
}

void test_array_remove(void) {
  array_t *array = make_test_array();
  __array_t *internal = (__array_t *)array;

  cmp_ok(array_remove(array, 2), "==", true,
         "returns true when the element was removed");

  char values[] = {'x', 'y', '1', '2', '3'};
  for (unsigned int i = 0; i < internal->len; i++) {
    cmp_ok((int)internal->state[i], "==", values[i],
           "contains all elements except for removed");
  }

  array_free(array);
}

void test_array_remove_not_found(void) {
  array_t *array = make_test_array();

  cmp_ok(array_remove(array, 12), "==", false,
         "returns false when the element was not removed");

  array_free(array);
}

#define MAPPER_INC 10
void *mapper(void *el, unsigned int index, array_t *array) {
  return el + MAPPER_INC;
}

void test_array_map(void) {
  array_t *array = make_test_array();
  __array_t *internal = (__array_t *)array;

  __array_t *transformed = (__array_t *)array_map(array, mapper);
  cmp_ok(transformed->len, "==", internal->len,
         "has the same length as the input array");

  for (unsigned int i = 0; i < 3; i++) {
    cmp_ok((int)transformed->state[i],
           "==", (int)internal->state[i] + MAPPER_INC,
           "applies the function to each element");
  }

  array_free(array);
  array_free(transformed);
}

bool filter(void *el, unsigned int index, array_t *array, void *compare_to) {
  return el > compare_to;
}
void test_array_filter(void) {
  array_t *array = make_test_array();

  __array_t *transformed =
      (__array_t *)array_filter(array, filter, (void *)'A');
  cmp_ok(transformed->len, "==", 3, "has a length of filtered elements only");
  for (unsigned int i = 0; i < transformed->len; i++) {
    void *el = array_get(array, i);

    if ((int)el > 'A') {
      cmp_ok((int)transformed->state[i], "==", (int)el,
             "includes only elements that match the filter condition");
    }
  }

  array_free(array);
  array_free(transformed);
}

void test_foreach_macro(void) {
  array_t *array = array_init();
  array_push(array, 1);
  array_push(array, 2);
  array_push(array, 3);

  foreach (array, x) {
    ok(x + 1 == array_get(array, x), "iterates the array");
  }

  unsigned int count = 0;
  foreach_i(array, x, 1) { count += 1; }

  ok(2 == count, "starts at the provided count");

  array_free(array);
}

void test_has_elements_macro(void) {
  array_t *array = NULL;

  lives_ok(
      { has_elements(array); },
      "has_elements does not segfault when provided a NULL array_t pointer");
  ok(has_elements(array) == 0, "has_elements returns false when arr is NULL");
  ok((!has_elements(array)) == 1,
     "negated has_elements returns true when arr is NULL");

  array = array_init();
  ok(has_elements(array) == 0,
     "has_elements returns false when arr has no elements");
  ok((!has_elements(array)) == 1,
     "negated has_elements returns true when arr has no elements");

  array_push(array, 1);
  ok(has_elements(array) == 1,
     "has_elements returns true when arr has elements");
  ok((!has_elements(array)) == 0,
     "negated has_elements returns false when arr has elements");

  array_free(array);
}

void test_array_collect(void) {
  array_t *collected = array_collect("a", "b", "c");

  ok(array_size(collected) == 3, "contains all and only the elements added");

  is("a", array_get(collected, 0), "has the expected element");
  is("b", array_get(collected, 1), "has the expected element");
  is("c", array_get(collected, 2), "has the expected element");

  array_free(collected);
}

int main() {
  plan(77);

  test_array_init();
  test_array_size();
  test_array_get();
  test_array_includes();
  test_array_push();
  test_array_pop();
  test_array_pop_empty();
  test_array_shift();
  test_array_shift_empty();
  test_array_slice();
  test_array_remove();
  test_array_remove_not_found();
  test_array_map();
  test_array_filter();
  test_array_find();

  // macros
  test_foreach_macro();
  test_has_elements_macro();
  test_array_collect();

  done_testing();
}

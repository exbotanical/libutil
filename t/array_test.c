#include <stdlib.h>

#include "libtap/libtap.h"
#include "libutil.h"
#include "tests.h"

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

  lives({ array = array_init(); }, "initializes array");
  ok(array_size(array) == 0, "initializes the array's length to zero");

  array_free(array, NULL);
}

void test_array_size(void) {
  array_t *array = array_init();

  ok(array_size(array) == 0, "retrieves initial size");
  array_push(array, "test");
  ok(array_size(array) == 1, "tracks size pointer");

  array_free(array, NULL);
}

void test_array_get(void) {
  array_t *array = array_init();
  is(array_get(array, 0), NULL, "returns NULL if empty");
  is(array_get(array, 4), NULL, "index larger than len returns NULL");
  is(array_get(array, -1), NULL, "-1 index returns NULL");
  is(array_get(array, -4), NULL, "negative index larger than len returns NULL");

  array_push(array, (void *)10);
  array_push(array, (void *)11);
  array_push(array, (void *)12);

  is(array_get(array, 4), NULL, "index larger than len returns NULL");
  ok((int)array_get(array, -1) == 12, "-1 index returns last element");
  ok((int)array_get(array, 1) == 11, "index returns correct element");
  ok((int)array_get(array, -2) == 11, "index returns correct element");

  array_free(array, NULL);
}

void test_array_includes(void) {
  array_t *array = make_test_array();

  ok(array_includes(array, int_comparator, (void *)'z') == true,
     "returns true when it includes the element");
  ok(array_includes(array, int_comparator, (void *)'n') == false,
     "returns false when it does not include the element");

  array_free(array, NULL);
}

void test_array_find(void) {
  array_t *array = make_test_array();
  ok((int)array_find(array, int_comparator, (void *)'z') == 2,
     "returns index when it finds the element");
  ok(array_find(array, int_comparator, (void *)'n') == -1,
     "returns -1 when it does not find the element");

  array_free(array, NULL);

  array_t *strarr = array_collect("92Q", "dew");
  ok((int)array_find(strarr, str_comparator, (void *)"dew") == 1,
     "returns index when it finds the string element");

  array_free(strarr, NULL);
}

void test_array_push(void) {
  __array_t *array = (__array_t *)array_init();

  char values[] = {'x', 'y', 'z'};
  for (unsigned int i = 0; i < 3; i++) {
    void *el = (void *)values[i];

    ok(array_push((array_t *)array, el) == true,
       "returns true when successful");
    ok(array->size == i + 1, "increases the array's length by one");
    ok((int)array->state[i] == (int)el,
       "adds the element to the end of the array");
  }

  array_free((array_t *)array, NULL);
}

void test_array_pop(void) {
  array_t *array = array_init();

  char values[] = {'x', 'y', 'z'};
  unsigned int i = 0;
  for (; i < 3; i++) {
    array_push(array, (void *)values[i]);
  }
  for (; i > 0; i--) {
    ok((int)array_pop(array) == values[i - 1],
       "removes the element from the end of the array");
    ok((int)((__array_t *)array)->size == i - 1,
       "decreases the array's length by one");
  }

  array_free(array, NULL);
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

  array_free(array, NULL);
}

void test_array_shift(void) {
  array_t *array = array_init();
  __array_t *internal = (__array_t *)array;

  char values[] = {'x', 'y', 'z'};
  for (unsigned int i = 0; i < 3; i++) {
    array_push(array, (void *)values[i]);
  }

  ok((int)array_shift(array) == 'x', "shift assert");
  ok(internal->size == 2, "shift assert");
  ok((int)internal->state[0] == 'y', "shift assert");
  ok((int)internal->state[1] == 'z', "shift assert");

  ok((int)array_shift(array) == 'y', "shift assert");
  ok((int)internal->size == 1, "shift assert");
  ok((int)internal->state[0] == 'z', "shift assert");

  ok((int)array_shift(array) == 'z', "shift assert");
  ok(internal->size == 0, "shift assert");

  array_free(array, NULL);
}

void test_array_shift_empty(void) {
  array_t *array = array_init();

  is(array_shift(array), NULL, "shift on an empty array returns NULL");

  array_free(array, NULL);
}

void test_array_slice(void) {
  array_t *array = make_test_array();
  __array_t *sliced = (__array_t *)array_slice(array, 1, 4);

  ok(sliced->size == 3, "slices the range of elements inclusively");

  for (unsigned int i = 0; i < 3; i++) {
    ok((int)sliced->state[i] == (int)((__array_t *)array)->state[i + 1],
       "contains the sliced elements");
  }

  array_free(array, NULL);
  array_free((array_t *)sliced, NULL);
}

void test_array_slice_negative(void) {
  array_t *array = make_test_array();
  __array_t *sliced = (__array_t *)array_slice(array, 1, -1);

  ok(sliced->size == 5, "slices the range of elements inclusively");

  for (unsigned int i = 0; i < 5; i++) {
    ok((int)sliced->state[i] == (int)((__array_t *)array)->state[i + 1],
       "contains the sliced elements");
  }

  array_free(array, NULL);
  array_free((array_t *)sliced, NULL);
}

void test_array_remove(void) {
  array_t *array = make_test_array();
  __array_t *internal = (__array_t *)array;

  ok(array_remove(array, 2) == true,
     "returns true when the element was removed");

  char values[] = {'x', 'y', '1', '2', '3'};
  for (unsigned int i = 0; i < internal->size; i++) {
    ok((int)internal->state[i] == values[i],
       "contains all elements except for removed");
  }

  array_free(array, NULL);
}

void test_array_remove_not_found(void) {
  array_t *array = make_test_array();

  ok(array_remove(array, 12) == false,
     "returns false when the element was not removed");

  array_free(array, NULL);
}

#define MAPPER_INC 10
void *mapper(void *el, unsigned int index, array_t *array) {
  return el + MAPPER_INC;
}

void test_array_map(void) {
  array_t *array = make_test_array();
  __array_t *internal = (__array_t *)array;

  __array_t *transformed = (__array_t *)array_map(array, mapper);
  ok(transformed->size == internal->size,
     "has the same length as the input array");

  for (unsigned int i = 0; i < 3; i++) {
    ok((int)transformed->state[i], "==", (int)internal->state[i] + MAPPER_INC,
       "applies the function to each element");
  }

  array_free(array, NULL);
  array_free((array_t *)transformed, NULL);
}

bool filter(void *el, unsigned int index, array_t *array, void *compare_to) {
  return el > compare_to;
}
void test_array_filter(void) {
  array_t *array = make_test_array();

  __array_t *transformed =
      (__array_t *)array_filter(array, filter, (void *)'A');
  ok(transformed->size == 3, "has a length of filtered elements only");
  for (unsigned int i = 0; i < transformed->size; i++) {
    void *el = array_get(array, i);

    if ((int)el > 'A') {
      ok((int)transformed->state[i] == (int)el,
         "includes only elements that match the filter condition");
    }
  }

  array_free(array, NULL);
  array_free((array_t *)transformed, NULL);
}

void test_foreach_macro(void) {
  array_t *array = array_init();
  array_push(array, (void *)1);
  array_push(array, (void *)2);
  array_push(array, (void *)3);

  foreach (array, x) {
    ok(x + 1 == array_get(array, x), "iterates the array");
  }

  unsigned int count = 0;
  foreach_i(array, x, 1) { count += 1; }

  ok(2 == count, "starts at the provided count");

  array_free(array, NULL);
}

void test_has_elements_macro(void) {
  array_t *array = NULL;

  lives(
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

  array_push(array, (void *)1);
  ok(has_elements(array) == 1,
     "has_elements returns true when arr has elements");
  ok((!has_elements(array)) == 0,
     "negated has_elements returns false when arr has elements");

  array_free(array, NULL);
}

void test_array_collect(void) {
  array_t *collected = array_collect("a", "b", "c");

  ok(array_size(collected) == 3, "contains all and only the elements added");

  is("a", array_get(collected, 0), "has the expected element");
  is("b", array_get(collected, 1), "has the expected element");
  is("c", array_get(collected, 2), "has the expected element");

  array_free(collected, NULL);
}

void test_array_concat(void) {
  char *v[6] = {"a", "b", "c", "x", "y", "z"};

  array_t *arr1 = array_collect(v[0], v[1], v[2]);
  array_t *arr2 = array_collect(v[3], v[4], v[5]);

  array_t *concatenated = array_concat(arr1, arr2);
  array_free(arr1, NULL);
  array_free(arr2, NULL);

  ok(array_size(concatenated) == 6, "has the expected size");
  foreach (concatenated, i) {
    is(array_get(concatenated, i), v[i], "contains %s at index %d", v[i], i);
  }

  array_free(concatenated, NULL);
}

void test_array_realloc_sanity(void) {
  void *v = "value";

  array_t *arr = array_init();
  for (int i = 0; i < 20; i++) {
    array_push(arr, v);
  }
  array_free(arr, NULL);
}

// bug fix
void test_array_get_negative_idx(void) {
  array_t *arr = array_init();
  array_push(arr, "x");

  is(array_get(arr, -1), "x",
     "retrieves the last (and only) element given index -1");

  array_push(arr, "y");

  is(array_get(arr, -1), "y", "retrieves the new last element given index -1");
  is(array_get(arr, -2), "x",
     "retrieves the penultimate element given index -2");

  array_free(arr, NULL);
}

void test_array_insert(void) {
  array_t *arr = array_init();

  array_insert(arr, 100, "xxx", NULL);
  array_free(arr, NULL);
}

void run_array_tests(void) {
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
  test_array_slice_negative();
  test_array_remove();
  test_array_remove_not_found();
  test_array_map();
  test_array_filter();
  test_array_find();
  test_array_concat();
  test_array_realloc_sanity();
  test_array_get_negative_idx();

  // macros
  test_foreach_macro();
  test_has_elements_macro();
  test_array_collect();
}

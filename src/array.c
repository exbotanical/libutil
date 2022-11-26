#include "array.h"

#include <errno.h>
#include <malloc.h>
#include <stdlib.h>

Array *array_init() {
  Array *array = malloc(sizeof(Array));
  if (!array) {
    errno = ENOMEM;
    return NULL;
  }

  array->state = malloc(sizeof(void *));
  if (!array->state) {
    errno = ENOMEM;
    return NULL;
  }

  array->len = 0;

  return array;
}

bool array_includes(Array *array, ComparatorFunction *comparator) {
  for (unsigned int i = 0; i < array->len; i++) {
    if (comparator(array->state[i])) {
      return true;
    }
  }

  return false;
}

bool array_push(Array *array, void *el) {
  void **next_state = realloc(array->state, (array->len + 1) * sizeof(void *));
  if (!next_state) {
    free(next_state);
    errno = ENOMEM;

    return false;
  }

  array->state = next_state;
  array->state[array->len++] = el;

  return true;
}

void *array_pop(Array *array) {
  int len = array->len;

  if (len > 0) {
    int next_len = len - 1;

    void *el = array->state[next_len];
    array->state[next_len] = NULL;
    array->len = next_len;

    return el;
  }

  return NULL;
}

Array *array_slice(Array *array, int start, int end) {
  Array *slice = array_init();
  int normalized_end = end == -1 ? (int)array->len : end;
  for (int i = start; i < normalized_end; i++) {
    array_push(slice, array->state[i]);
  }

  return slice;
}

Array *array_map(Array *array, CallbackFunction *callback) {
  Array *ret = array_init();
  for (unsigned int i = 0; i < array->len; i++) {
    array_push(ret, callback(array->state[i], i, array));
  }

  return ret;
}

Array *array_filter(Array *array, PredicateFunction *predicate) {
  Array *ret = array_init();
  for (unsigned int i = 0; i < array->len; i++) {
    void *el = array->state[i];
    if (predicate(el, i, array)) {
      array_push(ret, el);
    }
  }

  return ret;
}

void array_foreach(Array *array, CallbackFunction *callback) {
  for (unsigned int i = 0; i < array->len; i++) {
    callback(array->state[i], i, array);
  }
}

void array_free(Array *array) {
  free(array->state);
  array->state = NULL;
  free(array);
}

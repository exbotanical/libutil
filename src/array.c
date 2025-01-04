#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "libutil.h"

bool int_comparator(int a, int b) { return a == b; }
bool str_comparator(char *a, char *b) { return s_equals(a, b); }

size_t array_size(array_t *self) { return ((__array_t *)self)->size; }

void *array_get(array_t *self, ssize_t index) {
  __array_t *unwrapped = (__array_t *)self;
  if (unwrapped->size == 0) {
    return NULL;
  }

  size_t absolute = abs((int)index);
  if (absolute > unwrapped->size) {
    return NULL;
  }

  if (index < 0) {
    return unwrapped->state[unwrapped->size - absolute];
  }

  return unwrapped->state[index];
}

array_t *array_init(void) {
  __array_t *array = malloc(sizeof(__array_t));
  if (!array) {
    errno = ENOMEM;
    return NULL;
  }

  array->state = malloc(sizeof(void *));
  if (!array->state) {
    free(array);
    errno = ENOMEM;
    return NULL;
  }

  array->size = 0;
  array->capacity = 0;

  return (array_t *)array;
}

array_t *__array_collect(void *v, ...) {
  array_t *arr = array_init();

  va_list args;
  va_start(args, v);

  array_push(arr, v);
  while ((v = va_arg(args, void *))) {
    array_push(arr, v);
  }

  va_end(args);

  return arr;
}

bool array_includes(array_t *self, comparator_t *comparator, void *compare_to) {
  __array_t *unwrapped = (__array_t *)self;

  for (size_t i = 0; i < unwrapped->size; i++) {
    if (comparator(unwrapped->state[i], compare_to)) {
      return true;
    }
  }

  return false;
}

ssize_t array_find(array_t *self, comparator_t *comparator, void *compare_to) {
  __array_t *unwrapped = (__array_t *)self;

  for (size_t i = 0; i < unwrapped->size; i++) {
    if (comparator(unwrapped->state[i], compare_to)) {
      return i;
    }
  }

  return -1;
}

bool array_push(array_t *self, void *el) {
  __array_t *unwrapped = (__array_t *)self;

  if (unwrapped->size == unwrapped->capacity) {
    void **next_state = realloc(
        unwrapped->state,
        (unwrapped->size + LIB_UTIL_ARRAY_CAPACITY_INCR) * sizeof(void *));
    if (!next_state) {
      free(next_state);
      errno = ENOMEM;

      return false;
    }

    unwrapped->state = next_state;
    unwrapped->capacity += LIB_UTIL_ARRAY_CAPACITY_INCR;
  }

  unwrapped->state[unwrapped->size++] = el;

  return true;
}

bool array_insert(array_t *self, size_t index, void *el, free_fn *free_old_el) {
  __array_t *unwrapped = (__array_t *)self;

  if (unwrapped->capacity < index) {
    void **next_state = realloc(unwrapped->state, (index) * sizeof(void *));
    if (!next_state) {
      free(next_state);
      errno = ENOMEM;

      return false;
    }

    unwrapped->state = next_state;
    unwrapped->capacity = index;

    unwrapped->state[unwrapped->size++] = el;

    return true;
  }

  void *old_el = unwrapped->state[unwrapped->size++];
  if (old_el && free_old_el) {
    free_old_el(old_el);
  }

  unwrapped->state[unwrapped->size] = el;

  return true;
}

void *array_pop(array_t *self) {
  __array_t *unwrapped = (__array_t *)self;

  size_t size = unwrapped->size;

  if (size > 0) {
    size_t next_size = size - 1;

    void *el = unwrapped->state[next_size];
    unwrapped->state[next_size] = NULL;
    unwrapped->size = next_size;

    return el;
  }

  return NULL;
}

void *array_shift(array_t *self) {
  __array_t *unwrapped = (__array_t *)self;

  if (unwrapped->size == 0) {
    return false;
  }

  void *el = unwrapped->state[0];
  array_t *new = array_init();

  for (size_t i = 1; i < unwrapped->size; i++) {
    array_push(new, unwrapped->state[i]);
  }

  free(unwrapped->state);

  unwrapped->state = ((__array_t *)new)->state;  // Assign the new state
  unwrapped->size--;

  free(new);
  return el;
}

array_t *array_slice(array_t *self, size_t start, ssize_t end) {
  __array_t *unwrapped = (__array_t *)self;
  array_t *slice = array_init();

  // TODO: fix negative end beyond -1
  size_t normalized_end = end == -1 ? (int)unwrapped->size : end;
  if (end > (int)unwrapped->size) {
    return NULL;
  }

  for (size_t i = start; i < normalized_end; i++) {
    array_push(slice, unwrapped->state[i]);
  }

  return slice;
}

bool array_remove(array_t *self, size_t index) {
  __array_t *unwrapped = (__array_t *)self;

  if (unwrapped->size < index) {
    return false;
  }

  for (size_t i = 0; i < unwrapped->size - 1; i++) {
    if (i >= index) {
      unwrapped->state[i] = unwrapped->state[i + 1];
    }
  }

  unwrapped->size--;
  return true;
}

array_t *array_map(array_t *self, callback_t *callback) {
  __array_t *unwrapped = (__array_t *)self;

  array_t *ret = array_init();
  for (size_t i = 0; i < unwrapped->size; i++) {
    array_push(ret, callback(unwrapped->state[i], i, self));
  }

  return ret;
}

array_t *array_filter(array_t *self, predicate_t *predicate, void *compare_to) {
  __array_t *unwrapped = (__array_t *)self;

  array_t *ret = array_init();
  for (size_t i = 0; i < unwrapped->size; i++) {
    void *el = unwrapped->state[i];
    if (predicate(el, i, self, compare_to)) {
      array_push(ret, el);
    }
  }

  return ret;
}

void array_foreach(array_t *self, callback_t *callback) {
  __array_t *unwrapped = (__array_t *)self;

  for (size_t i = 0; i < unwrapped->size; i++) {
    callback(unwrapped->state[i], i, self);
  }
}

array_t *array_concat(array_t *arr1, array_t *arr2) {
  __array_t *internal1 = (__array_t *)arr1;
  __array_t *internal2 = (__array_t *)arr2;

  __array_t *result = (__array_t *)array_init();

  result->state = realloc(result->state,
                          (internal1->size + internal2->size) * sizeof(void *));
  if (!result->state) {
    errno = ENOMEM;
    return NULL;
  }

  result->size = internal1->size + internal2->size;

  memcpy(result->state, internal1->state, internal1->size * sizeof(void *));
  memcpy(result->state + internal1->size, internal2->state,
         internal2->size * sizeof(void *));

  return (array_t *)result;
}

void array_free(array_t *self, free_fn *free_fnptr) {
  __array_t *unwrapped = (__array_t *)self;
  if (free_fnptr) {
    foreach (self, i) {
      free_fnptr(array_get(self, i));
    }
  }
  free(unwrapped->state);
  unwrapped->state = NULL;
  free(unwrapped);
}

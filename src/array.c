#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "libutil.h"

bool int_comparator(int a, int b) { return a == b; }
bool str_comparator(char *a, char *b) { return s_equals(a, b); }

unsigned int array_size(array_t *array) { return ((__array_t *)array)->size; }

void *array_get(array_t *array, int index) {
  __array_t *internal = (__array_t *)array;
  if (internal->size == 0) {
    return NULL;
  }

  unsigned int absolute = abs(index);
  if (absolute > internal->size) {
    return NULL;
  }

  if (index < 0) {
    return internal->state[internal->size - absolute];
  }

  return internal->state[index];
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

bool array_includes(array_t *array, comparator_t *comparator,
                    void *compare_to) {
  __array_t *internal = (__array_t *)array;

  for (unsigned int i = 0; i < internal->size; i++) {
    if (comparator(internal->state[i], compare_to)) {
      return true;
    }
  }

  return false;
}

int array_find(array_t *array, comparator_t *comparator, void *compare_to) {
  __array_t *internal = (__array_t *)array;

  for (unsigned int i = 0; i < internal->size; i++) {
    if (comparator(internal->state[i], compare_to)) {
      return i;
    }
  }

  return -1;
}

bool array_push(array_t *array, void *el) {
  __array_t *internal = (__array_t *)array;

  if (internal->size == internal->capacity) {
    void **next_state = realloc(
        internal->state,
        (internal->size + LIB_UTIL_ARRAY_CAPACITY_INCR) * sizeof(void *));
    if (!next_state) {
      free(next_state);
      errno = ENOMEM;

      return false;
    }

    internal->state = next_state;
    internal->capacity += LIB_UTIL_ARRAY_CAPACITY_INCR;
  }

  internal->state[internal->size++] = el;

  return true;
}

bool array_insert(array_t *array, unsigned int index, void *el,
                  free_fn *free_old_el) {
  __array_t *internal = (__array_t *)array;

  if (internal->capacity < index) {
    void **next_state = realloc(internal->state, (index) * sizeof(void *));
    if (!next_state) {
      free(next_state);
      errno = ENOMEM;

      return false;
    }

    internal->state = next_state;
    internal->capacity = index;

    internal->state[internal->size++] = el;

    return true;
  }

  void *old_el = internal->state[internal->size++];
  if (old_el && free_old_el) {
    free_old_el(old_el);
  }

  internal->state[internal->size] = el;

  return true;
}

void *array_pop(array_t *array) {
  __array_t *internal = (__array_t *)array;

  unsigned int size = internal->size;

  if (size > 0) {
    unsigned int next_size = size - 1;

    void *el = internal->state[next_size];
    internal->state[next_size] = NULL;
    internal->size = next_size;

    return el;
  }

  return NULL;
}

void *array_shift(array_t *array) {
  __array_t *internal = (__array_t *)array;

  if (internal->size == 0) {
    return false;
  }

  void *el = internal->state[0];
  array_t *new = array_init();

  for (unsigned int i = 1; i < internal->size; i++) {
    array_push(new, internal->state[i]);
  }

  free(internal->state);

  internal->state = ((__array_t *)new)->state;  // Assign the new state
  internal->size--;

  free(new);
  return el;
}

array_t *array_slice(array_t *array, unsigned start, int end) {
  __array_t *internal = (__array_t *)array;
  array_t *slice = array_init();

  // TODO: fix negative end beyond -1
  unsigned int normalized_end = end == -1 ? (int)internal->size : end;
  if (end > (int)internal->size) {
    return NULL;
  }

  for (unsigned int i = start; i < normalized_end; i++) {
    array_push(slice, internal->state[i]);
  }

  return slice;
}

bool array_remove(array_t *array, unsigned int index) {
  __array_t *internal = (__array_t *)array;

  if (internal->size < index) {
    return false;
  }

  for (unsigned int i = 0; i < internal->size - 1; i++) {
    if (i >= index) {
      internal->state[i] = internal->state[i + 1];
    }
  }

  internal->size--;
  return true;
}

array_t *array_map(array_t *array, callback_t *callback) {
  __array_t *internal = (__array_t *)array;

  array_t *ret = array_init();
  for (unsigned int i = 0; i < internal->size; i++) {
    array_push(ret, callback(internal->state[i], i, array));
  }

  return ret;
}

array_t *array_filter(array_t *array, predicate_t *predicate,
                      void *compare_to) {
  __array_t *internal = (__array_t *)array;

  array_t *ret = array_init();
  for (unsigned int i = 0; i < internal->size; i++) {
    void *el = internal->state[i];
    if (predicate(el, i, array, compare_to)) {
      array_push(ret, el);
    }
  }

  return ret;
}

void array_foreach(array_t *array, callback_t *callback) {
  __array_t *internal = (__array_t *)array;

  for (unsigned int i = 0; i < internal->size; i++) {
    callback(internal->state[i], i, array);
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

void array_free(array_t *array, free_fn *free_fnptr) {
  __array_t *internal = (__array_t *)array;
  if (free_fnptr) {
    foreach (array, i) {
      free(array_get(array, i));
    }
  }
  free(internal->state);
  internal->state = NULL;
  free(internal);
}

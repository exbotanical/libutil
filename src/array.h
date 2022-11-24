#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdbool.h>

typedef struct array_t {
  void **state;
  size_t len;
} Array;

typedef void *CallbackFunction (void *el, int index, Array *array);
typedef bool *PredicateFunction (void *el, int index, Array *array);
typedef bool *ReducerFunction (Array *array, void *el, int index);

Array *array_init();

bool array_push(Array *array, void *el);

void *array_pop(Array *array);

Array *array_slice(Array *array, int start, int end);

Array *map(Array *array, CallbackFunction *callback);

Array *filter(Array *array, PredicateFunction *predicate);

Array *reduce(Array *array, ReducerFunction *reducer, Array *initial_arr);

void for_each(Array *array, CallbackFunction *callback);

void array_free(Array *array);

#endif /* ARRAY_H */

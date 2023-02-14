#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void **state;
  unsigned int len;
} array_t;

typedef struct array *Array; // TODO: opaque pointers

typedef void *CallbackFunction(void *el, unsigned int index, Array *array);
typedef bool PredicateFunction(void *el, unsigned int index, Array *array,
                               void *compare_to);
typedef bool ReducerFunction(Array *array, void *el, unsigned int index);
typedef bool ComparatorFunction(void *el, void *compare_to);

Array *array_init();

bool array_includes(Array *array, ComparatorFunction *comparator,
                    void *compare_to);

int array_find(Array *array, ComparatorFunction *comparator, void *compare_to);

bool array_push(Array *array, void *el);

void *array_pop(Array *array);

void *array_shift(Array *array);

Array *array_slice(Array *array, unsigned int start, int end);

bool array_remove(Array *array, unsigned int idx);

Array *array_map(Array *array, CallbackFunction *callback);

Array *array_filter(Array *array, PredicateFunction *predicate,
                    void *compare_to);

Array *array_reduce(Array *array, ReducerFunction *reducer, Array *initial_arr);

void array_foreach(Array *array, CallbackFunction *callback);

void array_free(Array *array);

#endif /* ARRAY_H */

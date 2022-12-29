#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct array_t {
  void **state;
  size_t len;
} Array;

typedef void *CallbackFunction(void *el, int index, Array *array);
typedef bool *PredicateFunction(void *el, int index, Array *array,
                                void *compare_to);
typedef bool *ReducerFunction(Array *array, void *el, int index);
typedef bool *ComparatorFunction(void *el, void *compare_to);

Array *array_init();

bool array_includes(Array *array, ComparatorFunction *comparator,
                    void *compare_to);

int array_find(Array *array, ComparatorFunction *comparator, void *compare_to);

bool array_push(Array *array, void *el);

void *array_pop(Array *array);

Array *array_slice(Array *array, int start, int end);

bool *array_remove(Array *array, int idx);

Array *array_map(Array *array, CallbackFunction *callback);

Array *array_filter(Array *array, PredicateFunction *predicate,
                    void *compare_to);

Array *array_reduce(Array *array, ReducerFunction *reducer, Array *initial_arr);

void array_foreach(Array *array, CallbackFunction *callback);

void array_free(Array *array);

#endif /* ARRAY_H */

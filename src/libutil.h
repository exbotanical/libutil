#ifndef LIBUTIL_H
#define LIBUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void **state;
  unsigned int len;
} array_t;

typedef array_t *Array;

typedef void *CallbackFunction(void *el, unsigned int index, Array *array);
typedef bool PredicateFunction(void *el, unsigned int index, Array *array,
                               void *compare_to);
typedef bool ReducerFunction(Array *array, void *el, unsigned int index);
typedef bool ComparatorFunction(void *el, void *compare_to);

unsigned int array_size(Array *array);

void *array_get(Array *array, int index);

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

unsigned int array_size(Array *array);

/**
 * @brief A dynamic string buffer.
 */
typedef struct {
  char *state;
  unsigned int len;
} buffer_t;

typedef buffer_t *Buffer;

int buffer_size(Buffer *buf);

char *buffer_state(Buffer *buf);

/**
 * @brief Allocates memory for a new buffer.
 *
 * @return Buffer*
 */
Buffer *buffer_init(const char *init);

/**
 * @brief Appends a string to a given buffer `buf`, reallocating the required
 * memory
 *
 * @param buf the buffer to which `s` will be appended
 * @param s char pointer to be appended to the buffer
 */
bool buffer_append(Buffer *buf, const char *s);

/**
 * @brief Appends a string to a given buffer `buf`, reallocating the required
 * memory
 *
 * @param buf the buffer to which `s` will be appended
 * @param s char pointer to be appended to the buffer
 * @param len the length at which to truncate the given string `s`
 */
bool buffer_append_with(Buffer *buf, const char *s, unsigned int len);

Buffer *buffer_concat(Buffer *buf_a, Buffer *buf_b);

/**
 * @brief Deallocate the dynamic memory used by an `Buffer`
 *
 * @param buf the buffer pointer
 */
void buffer_free(Buffer *buf);

char *fmt_str(char *fmt, ...);

char *str_truncate(const char *s, int n);

char *str_concat(const char *s1, const char *s2);

#ifdef __cplusplus
}
#endif

#endif /* LIBUTIL_H */

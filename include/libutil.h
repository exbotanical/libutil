#ifndef LIBUTIL_H
#define LIBUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifndef LIB_UTIL_ARRAY_CAPACITY_INCR
#define LIB_UTIL_ARRAY_CAPACITY_INCR 4
#endif

typedef struct {
  void **state;
  unsigned int size;
  unsigned int capacity;
} __array_t;

/**
 * array_t* represents an array of void pointers.
 */
typedef __array_t *array_t;

array_t *__array_collect(void *v, ...);

typedef void *callback_t(void *el, unsigned int index, array_t *array);
typedef bool predicate_t(void *el, unsigned int index, array_t *array,
                         void *compare_to);
typedef bool comparator_t(void *el, void *compare_to);
typedef void free_fn(void *el);

// An int comparator that implements the comparator_t interface
bool int_comparator(int a, int b);
// An char* comparator that implements the comparator_t interface
bool str_comparator(char *a, char *b);

/**
 * foreach generates a for loop statement to iterate the given array's size
 * where i is the variable in which the counter will be placed.
 *
 * Example:
 * foreach(array, x) { printf("%d\n", array_get(array, x) + 1); }
 */
#define foreach(arr, i) for (unsigned int i = 0; i < array_size(arr); i++)

/**
 * foreach_i generates a for loop statement to iterate the given array's size
 * where i is the variable in which the counter will be placed and start is the
 * beginning counter value.
 *
 * Example:
 * // start at index 3
 * foreach(array, x, 3) { printf("%d\n", array_get(array, x) + 1); }
 */
#define foreach_i(arr, i, start) \
  for (unsigned int i = start; i < array_size(arr); i++)

/**
 * has_elements returns a boolean indicating whether the given array_t contains
 * any elements. has_elements is NULL-safe.
 */
#define has_elements(arr) (arr != NULL && array_size(arr) > 0)

/**
 * array_collect collects the provided arguments into a new array whose elements
 * are the ordered arguments. No sentinel value is required.
 * Note: This method will NOT work with a list ending in NULL.
 *
 * Example: array_t* arr = array_collect("hello", "world");
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
#define array_collect(...) __array_collect(__VA_ARGS__, NULL)

/**
 * array_size returns the size of the given array.
 */
unsigned int array_size(array_t *array);

/**
 * array_get returns the element at the given index of the given array.
 * Returns NULL if index out-of-bounds.
 */
void *array_get(array_t *array, int index);

/**
 * array_init initializes and returns a new array_t*.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
array_t *array_init(void);

/**
 * array_includes accepts a comparator function `comparator` which it invokes
 * with each element of the array and `compare_to`. If the comparator returns
 * true, array_includes immediately returns true.
 *
 * If no match found, array_includes returns false.
 */
bool array_includes(array_t *array, comparator_t *comparator, void *compare_to);

/**
 * array_find accepts a comparator function `comparator` which it invokes
 * with each element of the array and `compare_to`. If the comparator returns
 * true, array_find immediately returns the index of the matched element.
 *
 * If no match found, array_find returns -1.
 */
int array_find(array_t *array, comparator_t *comparator, void *compare_to);

/**
 * array_push appends the given element to the end of the array.
 *
 * Set the `LIB_UTIL_ARRAY_CAPACITY_INCR` macro if you want to change the
 * default capacity incrementor. i.e. if using the default (4), the array
 * reallocates memory every 4 elements. If you know you'll be using more, you
 * can increase this amount and maintain a more performant array.
 */
bool array_push(array_t *array, void *el);

/**
 * array_pop removes the last element from the given array and returns that
 * element. This method changes the length of the array.
 */
void *array_pop(array_t *array);

/**
 * array_shift removes the first element from the given array and returns that
 * removed element. This method changes the length of the array.
 */
void *array_shift(array_t *array);

/**
 * array_slice returns a shallow copy of a portion of an array into a new
 * array object selected from start to end (end not included) where start and
 * end represent the indices of items in that array. The original array will not
 * be modified.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
array_t *array_slice(array_t *array, unsigned int start, int end);

/**
 * array_remove removes the element from the array at the given index. Returns a
 * bool indicating whether the element was removed (if not, it's likely the
 * given index is out-of-bounds). This method changes the length of the array
 * and collapses the elements.
 */
bool array_remove(array_t *array, unsigned int idx);

/**
 * array_map returns a copy of the given array after applying the provided
 * callback to each element.
 */
array_t *array_map(array_t *array, callback_t *callback);

/**
 * array_filter returns a new array with only elements for which the predicate
 * function returns true.
 */
array_t *array_filter(array_t *array, predicate_t *predicate, void *compare_to);

/**
 * array_foreach invokes the provided callback for each element in the given
 * array.
 */
void array_foreach(array_t *array, callback_t *callback);

/**
 * array_concat concatenates two arrays, returning a new array of [...arr1,
 * ...arr2]. The original arrays are not modified.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
array_t *array_concat(array_t *arr1, array_t *arr2);

/**
 * array_free frees the array and its internal state container. Safe to use
 * with an array of primitives. Accepts an optional function pointer if you want
 * all values to be freed.
 */
void array_free(array_t *array, free_fn *free_fnptr);

typedef struct {
  char *state;
  unsigned int len;
} __buffer_t;

typedef __buffer_t *buffer_t;

/**
 * buffer_size returns the length of the buffer.
 */
int buffer_size(buffer_t *buf);

/**
 * buffer_state returns the internal state of the buffer.
 */
char *buffer_state(buffer_t *buf);

/**
 * buffer_init initializes and returns a new buffer_t*.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
buffer_t *buffer_init(const char *init);

/**
 * buffer_append appends a string `s` to a given buffer `buf`, reallocating the
 * required memory as needed.
 */
bool buffer_append(buffer_t *buf, const char *s);

/**
 * buffer_append_with appends a string `s` to a given buffer `buf`, reallocating
 * the required memory as needed. A third parameter `len` specifies how many
 * characters of `s` to append.
 */
bool buffer_append_with(buffer_t *buf, const char *s, unsigned int len);

/**
 * buffer_concat concatenates two buffers and returns them as a new buffer. Does
 * not modify the given buffers.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
buffer_t *buffer_concat(buffer_t *buf_a, buffer_t *buf_b);

/**
 * buffer_free deallocates the dynamic memory used by a given buffer_t*.
 */
void buffer_free(buffer_t *buf);

/**
 * Returns a formatted string. Uses printf syntax.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_fmt(char *fmt, ...);

/**
 * s_truncate truncates the given string `s` by `n` characters.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_truncate(const char *s, int n);

/**
 * s_concat concatenates two strings `s1` and `s2` in that order.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_concat(const char *s1, const char *s2);

/**
 * s_copy returns a copy of given string `str`. Compare to strdup.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_copy(const char *s);

/**
 * s_indexof returns the index of a character `target`
 * as it exists in a character array `str`.
 * Returns The index of `target` qua `str`, or -1 if not extant.
 */
int s_indexof(const char *str, const char *target);

/**
 * s_substr finds and returns the substring between
 * indices `start` and `end` for a given string `str`.
 * The start index is always inclusive.
 * The third argument `inclusive` is a boolean flag indicating whether the
 * substring match should be end-inclusive.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_substr(const char *s, int start, int end, bool inclusive);

/**
 * s_casecmp performs a safe case-insensitive string comparison between
 * two strings and returns a bool indicating whether they are equal.
 */
bool s_casecmp(const char *s1, const char *s2);

/**
 * s_upper converts the given string `s` to uppercase.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_upper(const char *s);

/**
 * s_equals returns a bool indicating whether strings s1 and s2 are completely
 * equal (case-sensitive).
 */
bool s_equals(const char *s1, const char *s2);

/**
 * s_nullish tests whether a given string `s` is NULL or empty i.e. "".
 */
bool s_nullish(const char *s);

/**
 * s_trim returns a copy of the string `s` with all whitespace removed.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
char *s_trim(const char *s);

/**
 * s_split splits a string on all instances of a delimiter.
 * Returns an array_t* of matches, if any, or NULL if erroneous.
 *
 * Caller is responsible for `free`-ing the returned pointer.
 */
array_t *s_split(const char *s, const char *delim);

#ifndef READ_ALL_CHUNK_SZ
#define READ_ALL_CHUNK_SZ 262144
#endif

typedef enum {
  READ_ALL_OK = 0,          // Success
  READ_ALL_INVALID = -1,    // Bad input
  READ_ALL_ERR = -2,        // Stream err
  READ_ALL_TOO_LARGE = -3,  // Input too large
  READ_ALL_NOMEM = -4       // Out of memory
} read_all_result;

/**
 * TODO:
 *
 * @param fd
 * @return char*
 */
read_all_result read_all(FILE *fd, char **data_ptr, size_t *sz_ptr);

#ifdef __cplusplus
}
#endif

#endif /* LIBUTIL_H */

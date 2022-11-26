#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>

/**
 * @brief A dynamic string buffer.
 */
typedef struct buffer {
  char *state;
  int len;
} Buffer;

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

Buffer *buffer_concat(Buffer *buf_a, Buffer *buf_b);

/**
 * @brief Deallocate the dynamic memory used by an `Buffer`
 *
 * @param buf the buffer pointer
 */
void buffer_free(Buffer *buf);

#endif

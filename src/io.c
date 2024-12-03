#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libutil.h"

io_write_all_result io_write_all(FILE *fd, const char *data,
                                 size_t *n_write_ptr) {
  size_t sz = strlen(data);
  size_t total_written = 0;
  size_t n_written;

  if (fd == NULL || data == NULL) {
    return IO_WRITE_ALL_INVALID;
  }

  if (ferror(fd)) {
    return IO_WRITE_ALL_ERR;
  }

  while (total_written < sz) {
    size_t remaining = sz - total_written;
    size_t to_write = (remaining < IO_IO_WRITE_ALL_CHUNK_SZ)
                          ? remaining
                          : IO_IO_WRITE_ALL_CHUNK_SZ;

    n_written = fwrite(data + total_written, 1, to_write, fd);
    if (n_written == 0) {
      if (ferror(fd)) {
        return IO_WRITE_ALL_ERR;
      }
      break;
    }
    total_written += n_written;
  }
  // TODO: Tests
  *n_write_ptr = n_written;

  if (total_written < sz) {
    return IO_WRITE_ALL_INCOMPLETE;
  }

  if (fflush(fd) != 0) {
    return IO_WRITE_ALL_ERR;
  }

  return IO_WRITE_ALL_OK;
}

// Adapted from this answer https://stackoverflow.com/a/44894946
io_read_all_result io_read_all(FILE *fd, char **data_ptr, size_t *n_read_ptr) {
  char *data = NULL;
  char *tmp;
  size_t sz = 0;
  size_t used = 0;
  size_t n;

  if (fd == NULL || data_ptr == NULL || n_read_ptr == NULL) {
    return IO_READ_ALL_INVALID;
  }

  if (ferror(fd)) {
    return IO_READ_ALL_ERR;
  }

  while (true) {
    if (used + IO_IO_READ_ALL_CHUNK_SZ + 1 > sz) {
      sz = used + IO_IO_READ_ALL_CHUNK_SZ + 1;

      if (sz <= used) {
        free(data);
        return IO_READ_ALL_TOO_LARGE;
      }

      tmp = realloc(data, sz);
      if (tmp == NULL) {
        free(data);
        return IO_READ_ALL_NOMEM;
      }
      data = tmp;
    }

    n = fread(data + used, 1, IO_IO_READ_ALL_CHUNK_SZ, fd);
    if (n == 0) {
      break;
    }

    used += n;

    if (ferror(fd)) {
      free(data);
      return IO_READ_ALL_ERR;
    }

    tmp = realloc(data, used + 1);
    if (tmp == NULL) {
      free(data);
      return IO_READ_ALL_NOMEM;
    }

    data = tmp;
    data[used] = '\0';

    *data_ptr = realloc(data, sz);
    *n_read_ptr = used;

    return IO_READ_ALL_OK;
  }
}

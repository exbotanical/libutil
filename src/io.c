#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libutil.h"

// void write_all(int fd, const void *data, size_t sz) {
//   while (sz) {
//     ssize_t done;

//     done = write(fd, data, sz);
//     if (done <= 0) {
//       _exit(1);
//     }
//     data = (const char *)data + done;
//     sz -= done;
//   }
// }

// Adapted from this answer https://stackoverflow.com/a/44894946
read_all_result read_all(FILE *fd, char **data_ptr, size_t *sz_ptr) {
  char *data = NULL;
  char *tmp;
  size_t sz = 0;
  size_t used = 0;
  size_t n;

  if (fd == NULL || data_ptr == NULL || sz_ptr == NULL) {
    return READ_ALL_INVALID;
  }

  if (ferror(fd)) {
    return READ_ALL_ERR;
  }

  while (true) {
    if (used + READ_ALL_CHUNK_SZ + 1 > sz) {
      sz = used + READ_ALL_CHUNK_SZ + 1;

      if (sz <= used) {
        free(data);
        return READ_ALL_TOO_LARGE;
      }

      tmp = realloc(data, sz);
      if (tmp == NULL) {
        free(data);
        return READ_ALL_NOMEM;
      }
      data = tmp;
    }

    n = fread(data + used, 1, READ_ALL_CHUNK_SZ, fd);
    if (n == 0) {
      break;
    }

    used += n;

    if (ferror(fd)) {
      free(data);
      return READ_ALL_ERR;
    }

    tmp = realloc(data, used + 1);
    if (tmp == NULL) {
      free(data);
      return READ_ALL_NOMEM;
    }

    data = tmp;
    data[used] = '\0';

    *data_ptr = data;
    *sz_ptr = used;

    return READ_ALL_OK;
  }
}

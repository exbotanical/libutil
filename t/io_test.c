#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tests.h"

static void test_read_all_full_file() {
  FILE *fd = fopen("./t/fixtures/read.txt", "r");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  char *data = malloc(0);
  size_t sz = sizeof(data);
  io_read_all_result ret = io_read_all(fd, &data, &sz);

  eq_num(IO_READ_ALL_OK, ret, "returns IO_READ_ALL_OK on successful read");
  eq_num(sz, 68241, "reads the correct number of bytes");
  eq_str("The Colour Out of Space\n", s_substr(data, 0, 23, true),
         "first chars match");
  eq_str("troubling my sleep.\n", s_substr(data, 68221, 68241, true),
         "last chars match");

  fclose(fd);
  free(data);
}

static void test_write_all_full_file(void) {
  FILE *fd = fopen("./t/fixtures/read.txt", "r");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  char *data = malloc(0);
  size_t sz = sizeof(data);
  assert(IO_READ_ALL_OK == io_read_all(fd, &data, &sz));

  fclose(fd);
  fd = NULL;

  fd = fopen("./t/fixtures/write.txt", "w");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  size_t n_written = 0;
  eq_num(IO_WRITE_ALL_OK, io_write_all(fd, data, &n_written),
         "returns IO_WRITE_ALL_OK on successful write");
  eq_str("The Colour Out of Space\n", s_substr(data, 0, 23, true),
         "first chars match");
  eq_str("troubling my sleep.\n", s_substr(data, 68221, 68241, true),
         "last chars match");

  free(data);
  fclose(fd);
  fd = NULL;

  fd = fopen("./t/fixtures/write.txt", "r");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  data = malloc(0);
  sz = sizeof(data);
  assert(IO_READ_ALL_OK == io_read_all(fd, &data, &sz));

  eq_num(sz, 68241,
         "make sure the length is the same and we didn't append NULLs or "
         "something");

  fclose(fd);
  free(data);
  unlink("./t/fixtures/write.txt");
}

void run_io_tests(void) {
  test_read_all_full_file();
  test_write_all_full_file();
}

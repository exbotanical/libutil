#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libtap/libtap.h"
#include "libutil.h"
#include "tests.h"

void test_read_all_full_file() {
  FILE *fd = fopen("./t/fixtures/read.txt", "r");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  char *data = malloc(0);
  size_t sz = sizeof(data);
  read_all_result ret = read_all(fd, &data, &sz);

  ok(READ_ALL_OK == ret, "returns READ_ALL_OK on successful read");
  ok(sz == 68241, "reads the correct number of bytes");
  is("The Colour Out of Space\n", s_substr(data, 0, 23, true),
     "first chars match");
  is("troubling my sleep.\n", s_substr(data, 68221, 68241, true),
     "last chars match");

  fclose(fd);
  free(data);
}

void test_write_all_full_file(void) {
  FILE *fd = fopen("./t/fixtures/read.txt", "r");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  char *data = malloc(0);
  size_t sz = sizeof(data);
  assert(READ_ALL_OK == read_all(fd, &data, &sz));

  fclose(fd);
  fd = NULL;

  fd = fopen("./t/fixtures/write.txt", "w");
  if (!fd) {
    perror("fopen");
    exit(1);
  }

  ok(WRITE_ALL_OK == write_all(fd, data),
     "returns WRITE_ALL_OK on successful write");
  is("The Colour Out of Space\n", s_substr(data, 0, 23, true),
     "first chars match");
  is("troubling my sleep.\n", s_substr(data, 68221, 68241, true),
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
  assert(READ_ALL_OK == read_all(fd, &data, &sz));

  ok(sz == 68241,
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

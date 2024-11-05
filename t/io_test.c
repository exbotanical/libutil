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

  char *data = malloc(68300);
  size_t sz = sizeof(data);
  read_all_result ret = read_all(fd, &data, &sz);

  ok(READ_ALL_OK == ret, "returns READ_ALL_OK on successful read");
  ok(sz == 68241, "reads the correct number of bytes");
  is("The Colour Out of Space\n", s_substr(data, 0, 23, true),
     "first chars match");
  is("troubling my sleep.\n", s_substr(data, 68221, 68241, true),
     "last chars match");
}

void run_io_tests(void) { test_read_all_full_file(); }

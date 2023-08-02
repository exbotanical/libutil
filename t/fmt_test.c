#include "libutil.h"
#include "tap.c/tap.h"
#include "tests.h"

void test_fmt_str(void) {
  char *formatted = fmt_str("%s %d %s", "test", 11, "string");
  is(formatted, "test 11 string", "formats each part into a single string");

  free(formatted);
}

void run_fmt_tests(void) { test_fmt_str(); }

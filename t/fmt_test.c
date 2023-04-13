#include "libutil.h"
#include "tap.c/tap.h"

void test_fmt_str(void) {
  char *formatted = fmt_str("%s %d %s", "test", 11, "string");
  is(formatted, "test 11 string", "formats each part into a single string");
}

int main() {
  plan(1);

  test_fmt_str();

  done_testing();
}

#include "libutil.h"
#include "tap.c/tap.h"

void test_fmt_str() {
  char *formatted = fmt_str("%s %d %s", "test", 11, "string");
  is(formatted, "test 11 string", "formats each part into a single string");
}

int main(int argc, char *argv[]) {
  plan(1);

  test_fmt_str();

  done_testing();
}

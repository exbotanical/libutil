#include "tap.c/tap.h"
#include "tests.h"

int main() {
  plan(151);

  run_array_tests();
  run_buffer_tests();
  run_fmt_tests();
  run_str_tests();

  done_testing();
}

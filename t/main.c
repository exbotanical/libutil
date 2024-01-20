#include "tap.c/tap.h"
#include "tests.h"

int main() {
  plan(167);

  run_array_tests();
  run_buffer_tests();
  run_str_tests();

  done_testing();
}

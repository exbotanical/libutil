#include "libtap/libtap.h"
#include "tests.h"

int main() {
  plan(177);

  run_array_tests();
  run_buffer_tests();
  run_str_tests();
  run_io_tests();

  done_testing();
}

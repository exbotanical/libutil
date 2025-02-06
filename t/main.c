#include "libtap/libtap.h"
#include "tests.h"

int main() {
  plan(195);

  run_array_tests();
  run_buffer_tests();
  run_str_tests();
  run_io_tests();
  run_file_tests();

  done_testing();
}

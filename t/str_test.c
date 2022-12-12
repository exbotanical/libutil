#include "str.h"

#include "tap.c/tap.h"

void test_str_truncate() {
  char *string = "hello world";
  char *ret = str_truncate(string, 5);
  is(ret, "hello",
     "truncates the given string by the specified number of characters");
}

void test_str_truncate_too_long() {
  char *string = "hello world";
  char *ret = str_truncate(string, 15);
  is(ret, string, "returns the given string as-is");
}

void test_str_truncate_zero() {
  char *string = "hello world";
  char *ret = str_truncate(string, 0);
  is(ret, string, "returns the given string as-is");
}

void test_str_truncate_too_small() {
  char *string = "hello world";
  char *ret = str_truncate(string, -1);
  is(ret, string, "returns the given string as-is");
}

int main(int argc, char *argv[]) {
  plan(4);

  test_str_truncate();
  test_str_truncate_too_long();
  test_str_truncate_zero();
  test_str_truncate_too_small();

  done_testing();
}

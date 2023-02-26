#include "libutil.h"
#include "tap.c/tap.h"

void test_str_truncate_begin() {
  char *string = "hello world";
  char *ret = str_truncate(string, 5);
  is(ret, " world",
     "truncates the given string from the beginning by the specified number of "
     "characters");
}

void test_str_truncate_end() {
  char *string = "hello world";
  char *ret = str_truncate(string, -5);
  is(ret, "hello ",
     "truncates the given string from the end by the specified number of "
     "characters");
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

void test_str_truncate_too_short() {
  char *string = "hello world";
  char *ret = str_truncate(string, -15);
  is(ret, string, "returns the given string as-is");
}

void test_str_concat() {
  char *ret = str_concat("hello", " world");
  is(ret, "hello world", "concatenates the provided strings");
}

int main(int argc, char *argv[]) {
  plan(6);

  test_str_truncate_begin();
  test_str_truncate_end();
  test_str_truncate_too_long();
  test_str_truncate_zero();
  test_str_truncate_too_short();
  test_str_concat();

  done_testing();
}

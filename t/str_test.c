#include "libutil.h"
#include "tap.c/tap.h"

void test_s_truncate_begin() {
  char *string = "hello world";
  char *ret = s_truncate(string, 5);
  is(ret, " world",
     "truncates the given string from the beginning by the specified number of "
     "characters");
}

void test_s_truncate_end() {
  char *string = "hello world";
  char *ret = s_truncate(string, -5);
  is(ret, "hello ",
     "truncates the given string from the end by the specified number of "
     "characters");
}

void test_s_truncate() {
  char *file_path = "./routes/hello.c";
  char *ret = s_truncate(s_truncate(file_path, -2), 2);
  is(ret, "routes/hello", "truncates from both ends correctly");
}

void test_s_truncate_too_long() {
  char *string = "hello world";
  char *ret = s_truncate(string, 15);
  is(ret, string, "returns the given string as-is");
}

void test_s_truncate_zero() {
  char *string = "hello world";
  char *ret = s_truncate(string, 0);
  is(ret, string, "returns the given string as-is");
}

void test_s_truncate_too_short() {
  char *string = "hello world";
  char *ret = s_truncate(string, -15);
  is(ret, string, "returns the given string as-is");
}

void test_s_concat() {
  char *ret = s_concat("hello", " world");
  is(ret, "hello world", "concatenates the provided strings");
}

void test_s_indexof_ok() {
  char *test_str = "test_str";

  int idx = s_indexof(test_str, "_");
  ok(idx == 4, "finds the index of the target string");
}

void test_s_indexof_no_match() {
  char *test_str = "test_str";

  int idx = s_indexof(test_str, "/");
  ok(idx == -1, "returns -1 indicating no match");
}

void test_s_substr_ok() {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 4, 6, false);

  is(substring, "_s", "substring matches");
}

void test_s_substr_inclusive() {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 4, 6, true);

  is(substring, "_st", "inclusive substring matches");
}

void test_s_substr_no_range() {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 1, 1, false);

  is(substring, NULL, "substring sans range yields empty string");
}

void test_s_substr_no_range_inclusive() {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 1, 1, true);

  is(substring, "e",
     "inclusive substring sans range yields char at start index");
}

void test_s_casecmp() {
  char *s1 = "Content-Type";
  char *s2 = "Content-Type-";
  ok(s_casecmp(s1, s2) == 0, "compare s1: '%s' and s2: '%s' is false", s1, s2);
  ok(s_casecmp(s2, s1) == 0, "compare s2: '%s' and s1: '%s' is false", s2, s1);

  s2 = "Content-Type";
  ok(s_casecmp(s1, s2) == 1, "compare s1: '%s' and s2: '%s' is true", s1, s2);
  ok(s_casecmp(s2, s1) == 1, "compare s2: '%s' and s1: '%s' is true", s2, s1);

  s1 = "CONTENT-TYPE";
  ok(s_casecmp(s1, s2) == 1, "compare s1: '%s' and s2: '%s' is true", s1, s2);
  ok(s_casecmp(s2, s1) == 1, "compare s2: '%s' and s1: '%s' is true", s2, s1);

  s2 = "CoNtenT-TyPe";
  ok(s_casecmp(s1, s2) == 1, "compare s1: '%s' and s2: '%s' is true", s1, s2);
  ok(s_casecmp(s2, s1) == 1, "compare s2: '%s' and s1: '%s' is true", s2, s1);

  s1 = "CoNtenT_TyPe";
  ok(s_casecmp(s1, s2) == 0, "compare s1: '%s' and s2: '%s' is false", s1, s2);
  ok(s_casecmp(s2, s1) == 0, "compare s2: '%s' and s1: '%s' is false", s2, s1);

  s2 = "CoNtenT_Ty";
  ok(s_casecmp(s1, s2) == 0, "compare s1: '%s' and s2: '%s' is false", s1, s2);
  ok(s_casecmp(s2, s1) == 0, "compare s2: '%s' and s1: '%s' is false", s2, s1);
}

void test_s_upper() {
  char *expected = "HELLO";
  char *ret = s_upper("hello");
  is(expected, ret, "upper-cases the string");

  ret = s_upper("HELLO");
  is(expected, ret, "upper-cases an already upper-cased string");

  ret = s_upper("hEllO");
  is(expected, ret, "upper-cases a partially upper-cased str");
}

void test_s_equals() {
  const char *s1 = "hello";
  const char *s2 = "hello";

  ok(s_equals(s1, s2) == true, "strings are equal");
  ok(s_equals(s2, s1) == true, "strings are equal");
}

void test_s_equals_diff_case() {
  const char *s1 = "hello";
  const char *s2 = "Hello";

  ok(s_equals(s1, s2) == false, "strings are not equal when different case");
  ok(s_equals(s2, s1) == false, "strings are not equal when different case");
}

void test_s_equals_both_null() {
  ok(s_equals(NULL, NULL) == true, "strings are equal when both are NULL");
}

void test_s_equals_one_null() {
  const char *s1 = "hello";

  ok(s_equals(s1, NULL) == false, "strings are not equal when one is NULL");
  ok(s_equals(NULL, s1) == false, "strings are not equal when one is NULL");
}

void test_s_nullish() {
  const char *notempty = "hello";
  ok(s_nullish(notempty) == true,
     "returns true when testing a non-empty string");

  const char *empty = "";
  ok(s_nullish(empty) == false, "returns false when testing an empty string");

  const char *null;
  ok(s_nullish(null) == false, "returns false when testing an NULL string");
}

void test_s_trim() {
  is("cookie", s_trim(" cookie "),
     "trims a string with whitespace on either side");
  is("cookie", s_trim("cookie "),
     "trims a string with whitespace at the end only");
  is("cookie", s_trim(" cookie"),
     "trims a string with whitespace at the start only");
  is("cookie", s_trim("cookie"), "returns a string as-is when no whitespace");
  is("", s_trim(""), "returns a string as-is when empty");
  is("", s_trim("  "), "returns an empty string when only whitespace");

  is("cookie", s_trim("\tcookie\t"),
     "trims a string with ascii space on either side");
  is("cookie", s_trim("cookie\n"),
     "trims a string with ascii space at the end only");
  is("cookie", s_trim("\rcookie"),
     "trims a string with ascii space at the start only");
  is("", s_trim("\t\t\t\r\n"), "returns an empty string when all ascii space");
}

int main() {
  plan(45);

  test_s_truncate_begin();
  test_s_truncate_end();
  test_s_truncate();
  test_s_truncate_too_long();
  test_s_truncate_zero();
  test_s_truncate_too_short();

  test_s_concat();

  test_s_indexof_ok();
  test_s_indexof_no_match();

  test_s_substr_ok();
  test_s_substr_inclusive();
  test_s_substr_no_range();
  test_s_substr_no_range_inclusive();

  test_s_casecmp();

  test_s_upper();

  test_s_equals();
  test_s_equals_diff_case();
  test_s_equals_both_null();
  test_s_equals_one_null();

  test_s_trim();

  done_testing();
}

#include "libutil.h"
#include "tap.c/tap.h"

void test_s_truncate_begin(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, 5);
  is(ret, " world",
     "truncates the given string from the beginning by the specified number of "
     "characters");

  free(ret);
}

void test_s_truncate_end(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, -5);
  is(ret, "hello ",
     "truncates the given string from the end by the specified number of "
     "characters");

  free(ret);
}

void test_s_truncate(void) {
  char *file_path = "./routes/hello.c";
  char *intermediate = s_truncate(file_path, -2);
  char *ret = s_truncate(intermediate, 2);
  is(ret, "routes/hello", "truncates from both ends correctly");

  free(intermediate);
  free(ret);
}

void test_s_truncate_too_long(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, 15);
  is(ret, string, "returns the given string as-is");

  free(ret);
}

void test_s_truncate_zero(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, 0);
  is(ret, string, "returns the given string as-is");

  free(ret);
}

void test_s_truncate_too_short(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, -15);
  is(ret, string, "returns the given string as-is");

  free(ret);
}

void test_s_concat(void) {
  char *ret = s_concat("hello", " world");
  is(ret, "hello world", "concatenates the provided strings");

  free(ret);
}

void test_s_indexof_ok(void) {
  char *test_str = "test_str";

  int idx = s_indexof(test_str, "_");
  ok(idx == 4, "finds the index of the target string");
}

void test_s_indexof_no_match(void) {
  char *test_str = "test_str";

  int idx = s_indexof(test_str, "/");
  ok(idx == -1, "returns -1 indicating no match");
}

void test_s_substr_ok(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 4, 6, false);
  is(substring, "_s", "substring matches");

  free(substring);
}

void test_s_substr_inclusive(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 4, 6, true);

  is(substring, "_st", "inclusive substring matches");

  free(substring);
}

void test_s_substr_no_range(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 1, 1, false);

  is(substring, NULL, "substring sans range yields empty string");

  free(substring);
}

void test_s_substr_no_range_inclusive(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 1, 1, true);

  is(substring, "e",
     "inclusive substring sans range yields char at start index");

  free(substring);
}

void test_s_casecmp(void) {
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

void test_s_upper(void) {
  char *expected = "HELLO";
  char *ret = s_upper("hello");
  is(expected, ret, "upper-cases the string");

  free(ret);
  ret = s_upper("HELLO");
  is(expected, ret, "upper-cases an already upper-cased string");

  free(ret);
  ret = s_upper("hEllO");
  is(expected, ret, "upper-cases a partially upper-cased str");

  free(ret);
}

void test_s_equals(void) {
  const char *s1 = "hello";
  const char *s2 = "hello";

  ok(s_equals(s1, s2) == true, "strings are equal");
  ok(s_equals(s2, s1) == true, "strings are equal");
}

void test_s_equals_diff_case(void) {
  const char *s1 = "hello";
  const char *s2 = "Hello";

  ok(s_equals(s1, s2) == false, "strings are not equal when different case");
  ok(s_equals(s2, s1) == false, "strings are not equal when different case");
}

void test_s_equals_both_null(void) {
  ok(s_equals(NULL, NULL) == true, "strings are equal when both are NULL");
}

void test_s_equals_one_null(void) {
  const char *s1 = "hello";

  ok(s_equals(s1, NULL) == false, "strings are not equal when one is NULL");
  ok(s_equals(NULL, s1) == false, "strings are not equal when one is NULL");
}

void test_s_nullish(void) {
  const char *notempty = "hello";
  ok(s_nullish(notempty) == true,
     "returns true when testing a non-empty string");

  const char *empty = "";
  ok(s_nullish(empty) == false, "returns false when testing an empty string");

  const char *null;
  ok(s_nullish(null) == false, "returns false when testing an NULL string");
}

void test_s_trim(void) {
  char *ptr;
  is("cookie", (ptr = s_trim(" cookie ")),
     "trims a string with whitespace on either side");
  free(ptr);

  is("cookie", (ptr = s_trim("cookie ")),
     "trims a string with whitespace at the end only");
  free(ptr);

  is("cookie", (ptr = s_trim(" cookie")),
     "trims a string with whitespace at the start only");
  free(ptr);

  is("cookie", (ptr = s_trim("cookie")),
     "returns a string as-is when no whitespace");
  free(ptr);

  is("", (ptr = s_trim("")), "returns a string as-is when empty");
  free(ptr);

  is("", (ptr = s_trim("  ")), "returns an empty string when only whitespace");
  free(ptr);

  is("cookie", (ptr = s_trim("\tcookie\t")),
     "trims a string with ascii space on either side");
  free(ptr);

  is("cookie", (ptr = s_trim("cookie\n")),
     "trims a string with ascii space at the end only");
  free(ptr);

  is("cookie", (ptr = s_trim("\rcookie")),
     "trims a string with ascii space at the start only");
  free(ptr);

  is("", (ptr = s_trim("\t\t\t\r\n")),
     "returns an empty string when all ascii space");
  free(ptr);
}

void test_s_copy(void) {
  const char *v = "hello";
  char *cp = s_copy(v);
  is(v, cp, "s_copy copies the string and does not mangle it");

  free(cp);
}

void test_s_split_ok(void) {
  char *test_str = "aa:b:c:d";
  array_t *paths = s_split(test_str, ":");

  ok(array_size(paths) == 4, "only the matched characters are captured");
  is(array_get(paths, 0), "aa", "substring is captured");
  is(array_get(paths, 1), "b", "substring is captured");
  is(array_get(paths, 2), "c", "substring is captured");
  is(array_get(paths, 3), "d", "substring is captured");

  array_free_ptrs(paths);
}

void test_s_split_no_match(void) {
  char *test_str = "a:b:c:d";
  array_t *paths = s_split(test_str, "/");

  ok(array_size(paths) == 0, "split returns if delimiter not extant");

  array_free(paths);
}

void test_s_split_empty_input(void) {
  char *test_str = "";
  array_t *paths = s_split(test_str, "/");

  ok(array_size(paths) == 0, "split returns if input is empty");

  array_free(paths);
}

void test_s_split_end_match(void) {
  char *test_str = "a:b:c:d/test";
  array_t *paths = s_split(test_str, "/");

  ok(array_size(paths) == 2, "only the matched characters are captured");
  is(array_get(paths, 0), "a:b:c:d", "substring is captured");
  is(array_get(paths, 1), "test", "substring is captured");

  array_free(paths);
}

int main() {
  plan(56);

  test_s_copy();

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

  test_s_split_ok();
  test_s_split_no_match();
  test_s_split_empty_input();
  test_s_split_end_match();

  done_testing();
}

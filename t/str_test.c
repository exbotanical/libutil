#include <stdlib.h>
#include <string.h>

#include "tests.h"

static void test_s_truncate_begin(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, 5);
  eq_str(ret, " world",
         "truncates the given string from the beginning by the specified "
         "number of "
         "characters");

  free(ret);
}

static void test_s_truncate_end(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, -5);
  eq_str(ret, "hello ",
         "truncates the given string from the end by the specified number of "
         "characters");

  free(ret);
}

static void test_s_truncate(void) {
  char *file_path = "./routes/hello.c";
  char *intermediate = s_truncate(file_path, -2);
  char *ret = s_truncate(intermediate, 2);
  eq_str(ret, "routes/hello", "truncates from both ends correctly");

  free(intermediate);
  free(ret);
}

static void test_s_truncate_too_long(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, 15);
  eq_str(ret, string, "returns the given string as-is");

  free(ret);
}

static void test_s_truncate_zero(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, 0);
  eq_str(ret, string, "returns the given string as-is");

  free(ret);
}

static void test_s_truncate_too_short(void) {
  char *string = "hello world";
  char *ret = s_truncate(string, -15);
  eq_str(ret, string, "returns the given string as-is");

  free(ret);
}

static void test_s_concat(void) {
  char *ret = s_concat("hello", " world");
  eq_str(ret, "hello world", "concatenates the provided strings");
  free(ret);
}

static void test_s_concat_arr(void) {
  char **arr = malloc(sizeof(char *) * 4);

  arr[0] = "Hassle";
  arr[1] = "Goldstriker";
  arr[2] = "B.U.R.T";
  arr[3] = NULL;

  char *ret = s_concat_arr(arr, ", ");
  eq_str(ret, "Hassle, Goldstriker, B.U.R.T",
         "concatenates the provided array of strings with the given delimiter");

  free(ret);
  free(arr);
}

static void test_s_indexof_ok(void) {
  char *test_str = "test_str";

  size_t idx = s_indexof(test_str, "_");
  eq_num(idx, 4, "finds the index of the target string");
}

static void test_s_indexof_no_match(void) {
  char *test_str = "test_str";

  size_t idx = s_indexof(test_str, "/");
  eq_num(idx, -1, "returns -1 indicating no match");
}

static void test_s_substr_ok(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 4, 6, false);
  eq_str(substring, "_s", "substring matches");

  free(substring);
}

static void test_s_substr_inclusive(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 4, 6, true);

  eq_str(substring, "_st", "inclusive substring matches");

  free(substring);
}

static void test_s_substr_no_range(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 1, 1, false);

  eq_null(substring, NULL, "substring sans range yields empty string");

  free(substring);
}

static void test_s_substr_no_range_inclusive(void) {
  char *test_str = "test_str";

  char *substring = s_substr(test_str, 1, 1, true);

  eq_str(substring, "e",
         "inclusive substring sans range yields char at start index");

  free(substring);
}

static void test_s_casecmp(void) {
  char *s1 = "Content-Type";
  char *s2 = "Content-Type-";
  eq_num(s_casecmp(s1, s2), 0, "compare s1: '%s' and s2: '%s' is false", s1,
         s2);
  eq_num(s_casecmp(s2, s1), 0, "compare s2: '%s' and s1: '%s' is false", s2,
         s1);

  s2 = "Content-Type";
  eq_num(s_casecmp(s1, s2), 1, "compare s1: '%s' and s2: '%s' is true", s1, s2);
  eq_num(s_casecmp(s2, s1), 1, "compare s2: '%s' and s1: '%s' is true", s2, s1);

  s1 = "CONTENT-TYPE";
  eq_num(s_casecmp(s1, s2), 1, "compare s1: '%s' and s2: '%s' is true", s1, s2);
  eq_num(s_casecmp(s2, s1), 1, "compare s2: '%s' and s1: '%s' is true", s2, s1);

  s2 = "CoNtenT-TyPe";
  eq_num(s_casecmp(s1, s2), 1, "compare s1: '%s' and s2: '%s' is true", s1, s2);
  eq_num(s_casecmp(s2, s1), 1, "compare s2: '%s' and s1: '%s' is true", s2, s1);

  s1 = "CoNtenT_TyPe";
  eq_num(s_casecmp(s1, s2), 0, "compare s1: '%s' and s2: '%s' is false", s1,
         s2);
  eq_num(s_casecmp(s2, s1), 0, "compare s2: '%s' and s1: '%s' is false", s2,
         s1);

  s2 = "CoNtenT_Ty";
  eq_num(s_casecmp(s1, s2), 0, "compare s1: '%s' and s2: '%s' is false", s1,
         s2);
  eq_num(s_casecmp(s2, s1), 0, "compare s2: '%s' and s1: '%s' is false", s2,
         s1);
}

static void test_s_upper(void) {
  char *expected = "HELLO";
  char *ret = s_upper("hello");
  eq_str(expected, ret, "upper-cases the string");

  free(ret);
  ret = s_upper("HELLO");
  eq_str(expected, ret, "upper-cases an already upper-cased string");

  free(ret);
  ret = s_upper("hEllO");
  eq_str(expected, ret, "upper-cases a partially upper-cased str");

  free(ret);
}

static void test_s_equals(void) {
  const char *s1 = "hello";
  const char *s2 = "hello";

  eq_true(s_equals(s1, s2), "strings are equal");
  eq_true(s_equals(s2, s1), "strings are equal");
}

static void test_s_equals_diff_case(void) {
  const char *s1 = "hello";
  const char *s2 = "Hello";

  eq_false(s_equals(s1, s2), "strings are not equal when different case");
  eq_false(s_equals(s2, s1), "strings are not equal when different case");
}

static void test_s_equals_both_null(void) {
  eq_true(s_equals(NULL, NULL), "strings are equal when both are NULL");
}

static void test_s_equals_one_null(void) {
  const char *s1 = "hello";

  eq_false(s_equals(s1, NULL), "strings are not equal when one is NULL");
  eq_false(s_equals(NULL, s1), "strings are not equal when one is NULL");
}

static void test_s_nullish(void) {
  const char *notempty = "hello";
  eq_true(s_nullish(notempty), "returns true when testing a non-empty string");

  const char *empty = "";
  eq_false(s_nullish(empty), "returns false when testing an empty string");

  const char *null;
  eq_false(s_nullish(null), "returns false when testing an NULL string");
}

static void test_s_trim(void) {
  char *ptr;
  eq_str("cookie", (ptr = s_trim(" cookie ")),
         "trims a string with whitespace on either side");
  free(ptr);

  eq_str("cookie", (ptr = s_trim("cookie ")),
         "trims a string with whitespace at the end only");
  free(ptr);

  eq_str("cookie", (ptr = s_trim(" cookie")),
         "trims a string with whitespace at the start only");
  free(ptr);

  eq_str("cookie", (ptr = s_trim("cookie")),
         "returns a string as-is when no whitespace");
  free(ptr);

  eq_str("", (ptr = s_trim("")), "returns a string as-is when empty");
  free(ptr);

  eq_str("", (ptr = s_trim("  ")),
         "returns an empty string when only whitespace");
  free(ptr);

  eq_str("cookie", (ptr = s_trim("\tcookie\t")),
         "trims a string with ascii space on either side");
  free(ptr);

  eq_str("cookie", (ptr = s_trim("cookie\n")),
         "trims a string with ascii space at the end only");
  free(ptr);

  eq_str("cookie", (ptr = s_trim("\rcookie")),
         "trims a string with ascii space at the start only");
  free(ptr);

  eq_str("", (ptr = s_trim("\t\t\t\r\n")),
         "returns an empty string when all ascii space");
  free(ptr);
}

static void test_s_copy(void) {
  const char *v = "hello";
  char *cp = s_copy(v);
  eq_str(v, cp, "s_copy copies the string and does not mangle it");

  free(cp);
}

static void test_s_split_ok(void) {
  char *test_str = "aa:b:c:d";
  array_t *paths = s_split(test_str, ":");

  eq_num(array_size(paths), 4, "only the matched characters are captured");
  eq_str(array_get(paths, 0), "aa", "substring is captured");
  eq_str(array_get(paths, 1), "b", "substring is captured");
  eq_str(array_get(paths, 2), "c", "substring is captured");
  eq_str(array_get(paths, 3), "d", "substring is captured");

  array_free(paths, free);
}

static void test_s_split_no_match(void) {
  char *test_str = "a:b:c:d";
  array_t *paths = s_split(test_str, "/");

  eq_num(array_size(paths), 0, "split returns if delimiter not extant");

  array_free(paths, free);
}

static void test_s_split_empty_input(void) {
  char *test_str = "";
  array_t *paths = s_split(test_str, "/");

  eq_num(array_size(paths), 0, "split returns if input is empty");

  array_free(paths, NULL);
}

static void test_s_split_end_match(void) {
  char *test_str = "a:b:c:d/test";
  array_t *paths = s_split(test_str, "/");

  eq_num(array_size(paths), 2, "only the matched characters are captured");
  eq_str(array_get(paths, 0), "a:b:c:d", "substring is captured");
  eq_str(array_get(paths, 1), "test", "substring is captured");

  array_free(paths, free);
}

static void test_s_fmt(void) {
  char *formatted = s_fmt("%s %d %s", "test", 11, "string");
  eq_str(formatted, "test 11 string", "formats each part into a single string");

  free(formatted);
}

void run_str_tests(void) {
  test_s_copy();

  test_s_truncate_begin();
  test_s_truncate_end();
  test_s_truncate();
  test_s_truncate_too_long();
  test_s_truncate_zero();
  test_s_truncate_too_short();

  test_s_concat();
  test_s_concat_arr();

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

  test_s_fmt();
}

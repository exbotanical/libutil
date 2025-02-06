#include <stdio.h>
#include <string.h>

#include "tests.h"

static void test_buffer_init(void) {
  buffer_t *buf = buffer_init(NULL);

  eq_null(((__buffer_t *)buf)->state, NULL,
          "newly initialized buffer's state is NULL");
  eq_num(((__buffer_t *)buf)->len, 0, "newly initialized buffer's length is 0");

  buffer_free(buf);
}

static void test_buffer_size(void) {
  buffer_t *buf = buffer_init(NULL);
  eq_num(buffer_size(buf), 0, "retrieves initial size");

  buffer_append(buf, "hello");
  eq_num(buffer_size(buf), 5, "retrieves updated size");

  buffer_free(buf);
}

static void test_buffer_state(void) {
  const char *v = "hello";

  buffer_t *buf = buffer_init(NULL);
  eq_null(buffer_state(buf), NULL, "initial state is NULL");

  buffer_append(buf, v);
  eq_str(buffer_state(buf), v, "initial state is %s", v);

  buffer_free(buf);
}

static void test_buffer_init_with_initial(void) {
  char *test_str = "test";
  size_t test_str_len = strlen(test_str);

  buffer_t *buf = buffer_init(test_str);

  eq_str(((__buffer_t *)buf)->state, test_str,
         "newly initialized buffer's state equals %s", test_str);
  eq_num(((__buffer_t *)buf)->len, test_str_len,
         "newly initialized buffer's length is %d", test_str_len);

  buffer_free(buf);
}

static void test_buffer_append(void) {
  buffer_t *buf = buffer_init(NULL);

  buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");
  buffer_append(buf, "12345678910");
  buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");

  char *expected =
      "abcdefghijklmnopqrstuvwxyz12345678910abcdefghijklmnopqrstuvwxyz";

  eq_str(((__buffer_t *)buf)->state, expected,
         "buffer holds all appended characters in order");
  size_t expected_len = strlen(expected);
  eq_num(((__buffer_t *)buf)->len, strlen(expected), "buffer's length is %d",
         expected_len);

  buffer_free(buf);
}

static void test_buffer_append_with(void) {
  buffer_t *buf = buffer_init(NULL);

  buffer_append_with(buf, "abcdefghijklmnopqrstuvwxyz", 10);
  buffer_append_with(buf, "12345678910", 5);
  buffer_append_with(buf, "abcdefghijklmnopqrstuvwxyz", 10);

  char *expected = "abcdefghij12345abcdefghij";

  eq_num(strcmp(buffer_state(buf), expected), 0,
         "buffer contains all appended characters in order");
  size_t expected_len = strlen(expected);
  eq_num(((__buffer_t *)buf)->len, expected_len, "buffer's length is %d",
         expected_len);

  buffer_free(buf);
}

static void test_buffer_concat(void) {
  buffer_t *buf_a = buffer_init("test");
  buffer_t *buf_b = buffer_init("string");
  buffer_t *buf_c = buffer_concat(buf_a, buf_b);

  eq_str(((__buffer_t *)buf_c)->state, "teststring",
         "concatenates the two buffers' states");
  size_t expected_len = strlen("teststring");
  eq_num(((__buffer_t *)buf_c)->len, expected_len, "buffer's length is %d",
         expected_len);

  buffer_free(buf_a);
  buffer_free(buf_b);
  buffer_free(buf_c);
}

static void test_buffer_concat_on_null(void) {
  buffer_t *buf_a = buffer_init(NULL);
  buffer_t *buf_b = buffer_init(NULL);

  buffer_t *buf_c = buffer_concat(buf_a, buf_b);

  eq_null(buf_c, NULL, "returns NULL");

  buffer_free(buf_a);
  buffer_free(buf_b);
}

static void test_buffer_free(void) {
  buffer_t *buf = buffer_init(NULL);
  lives({ buffer_free(buf); }, "frees the buffer's heap memory");
}

static void test_buffer_free_nonnull(void) {
  buffer_t *buf = buffer_init("test");
  lives({ buffer_free(buf); }, "frees the buffer's memory");
}

static void test_buffer_slice_ok(void) {
  buffer_t *buf = buffer_init("test world test");

  char slice[32];

  eq_true(buffer_slice(buf, 5, 9, slice),
          "retval is true indicating no errors");
  eq_str(slice, "world", "returns the expected slice");
}

static void test_buffer_slice_empty_buffer(void) {
  buffer_t *buf = buffer_init("");

  char slice[32];
  eq_true(buffer_slice(buf, 0, 0, slice),
          "retval is zero indicating no errors");
  eq_str(slice, "", "returns the expected slice");
}

static void test_buffer_slice_bad_range(void) {
  buffer_t *buf = buffer_init("test world test");

  char slice[32];
  // No segfault from this proves the function fast-fails
  eq_false(buffer_slice(buf, 5, 32, NULL),
           "retval is false indicating a bad range");
}

static void test_buffer_slice_null_buffer(void) {
  buffer_t *buf = buffer_init(NULL);

  char slice[32];
  eq_false(buffer_slice(buf, 0, 0, NULL),
           "retval is false indicating a NULL internal state");
}

void run_buffer_tests(void) {
  test_buffer_init();
  test_buffer_init_with_initial();

  test_buffer_free();
  test_buffer_free_nonnull();

  test_buffer_size();

  test_buffer_state();

  test_buffer_append();
  test_buffer_append_with();

  test_buffer_concat();
  test_buffer_concat_on_null();

  test_buffer_slice_ok();
  test_buffer_slice_empty_buffer();
  test_buffer_slice_bad_range();
  test_buffer_slice_null_buffer();
}

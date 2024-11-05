#include <stdio.h>
#include <string.h>

#include "libtap/libtap.h"
#include "libutil.h"
#include "tests.h"

void test_buffer_init(void) {
  buffer_t *buf = buffer_init(NULL);

  is(((__buffer_t *)buf)->state, NULL,
     "newly initialized buffer's state is NULL");
  ok(((__buffer_t *)buf)->len == 0, "newly initialized buffer's length is 0");

  buffer_free(buf);
}

void test_buffer_size(void) {
  buffer_t *buf = buffer_init(NULL);
  ok(buffer_size(buf) == 0, "retrieves initial size");

  buffer_append(buf, "hello");
  ok(buffer_size(buf) == 5, "retrieves updated size");

  buffer_free(buf);
}

void test_buffer_state(void) {
  const char *v = "hello";

  buffer_t *buf = buffer_init(NULL);
  is(buffer_state(buf), NULL, "initial state is NULL");

  buffer_append(buf, v);
  is(buffer_state(buf), v, "initial state is %s", v);

  buffer_free(buf);
}

void test_buffer_init_with_initial(void) {
  char *test_str = "test";
  unsigned int test_str_len = strlen(test_str);

  buffer_t *buf = buffer_init(test_str);

  is(((__buffer_t *)buf)->state, test_str,
     "newly initialized buffer's state equals %s", test_str);
  ok(((__buffer_t *)buf)->len == test_str_len,
     "newly initialized buffer's length is %d", test_str_len);

  buffer_free(buf);
}

void test_buffer_append(void) {
  buffer_t *buf = buffer_init(NULL);

  buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");
  buffer_append(buf, "12345678910");
  buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");

  char *expected =
      "abcdefghijklmnopqrstuvwxyz12345678910abcdefghijklmnopqrstuvwxyz";

  is(((__buffer_t *)buf)->state, expected,
     "buffer holds all appended characters in order");
  unsigned int expected_len = strlen(expected);
  ok(((__buffer_t *)buf)->len == strlen(expected), "buffer's length is %d",
     expected_len);

  buffer_free(buf);
}

void test_buffer_append_with(void) {
  buffer_t *buf = buffer_init(NULL);

  buffer_append_with(buf, "abcdefghijklmnopqrstuvwxyz", 10);
  buffer_append_with(buf, "12345678910", 5);
  buffer_append_with(buf, "abcdefghijklmnopqrstuvwxyz", 10);

  char *expected = "abcdefghij12345abcdefghij";

  ok(strcmp(buffer_state(buf), expected) == 0,
     "buffer contains all appended characters in order");
  unsigned int expected_len = strlen(expected);
  ok(((__buffer_t *)buf)->len == expected_len, "buffer's length is %d",
     expected_len);

  buffer_free(buf);
}

void test_buffer_concat(void) {
  buffer_t *buf_a = buffer_init("test");
  buffer_t *buf_b = buffer_init("string");
  buffer_t *buf_c = buffer_concat(buf_a, buf_b);

  is(((__buffer_t *)buf_c)->state, "teststring",
     "concatenates the two buffers' states");
  unsigned int expected_len = strlen("teststring");
  ok(((__buffer_t *)buf_c)->len == expected_len, "buffer's length is %d",
     expected_len);

  buffer_free(buf_a);
  buffer_free(buf_b);
  buffer_free(buf_c);
}

void test_buffer_concat_on_null(void) {
  buffer_t *buf_a = buffer_init(NULL);
  buffer_t *buf_b = buffer_init(NULL);

  buffer_t *buf_c = buffer_concat(buf_a, buf_b);

  is(buf_c, NULL, "returns NULL");

  buffer_free(buf_a);
  buffer_free(buf_b);
}

void test_buffer_free(void) {
  buffer_t *buf = buffer_init(NULL);
  lives({ buffer_free(buf); }, "frees the buffer's heap memory");
}

void test_buffer_free_nonnull(void) {
  buffer_t *buf = buffer_init("test");
  lives({ buffer_free(buf); }, "frees the buffer's memory");
}

void test_buffer_slice_ok(void) {
  buffer_t *buf = buffer_init("test world test");

  char slice[32];
  int ret = buffer_slice(buf, 5, 9, slice);
  ok(ret == 0, "retval is zero indicating no errors");
  is(slice, "world", "returns the expected slice");
}

void test_buffer_slice_empty_buffer(void) {
  buffer_t *buf = buffer_init("");

  char slice[32];
  int ret = buffer_slice(buf, 0, 0, slice);
  ok(ret == 0, "retval is zero indicating no errors");
  is(slice, "", "returns the expected slice");
}

void test_buffer_slice_bad_range(void) {
  buffer_t *buf = buffer_init("test world test");

  char slice[32];
  int ret = buffer_slice(
      buf, 5, 32,
      NULL);  // No segfault from this proves the function fast-fails
  ok(ret == -1, "retval is -1 indicating a bad range");
}

void test_buffer_slice_null_buffer(void) {
  buffer_t *buf = buffer_init(NULL);

  char slice[32];
  int ret = buffer_slice(buf, 0, 0, NULL);
  ok(ret == -1, "retval is -1 indicating a NULL internal state");
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

#include "buffer.h"

#include <stdio.h>
#include <string.h>

#include "tap.c/tap.h"

void test_buffer_init() {
  Buffer *buf = buffer_init(NULL);

  is(buf->state, NULL, "newly initialized buffer's state is NULL");
  ok(buf->len == 0, "newly initialized buffer's length is 0");
}

void test_buffer_init_with_initial() {
  char *test_str = "test";
  int test_str_len = strlen(test_str);

  Buffer *buf = buffer_init(test_str);

  is(buf->state, test_str, "newly initialized buffer's state equals %s",
     test_str);
  ok(buf->len == test_str_len, "newly initialized buffer's length is %d",
     test_str_len);
}

void test_buffer_append() {
  Buffer *buf = buffer_init(NULL);

  buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");
  buffer_append(buf, "12345678910");
  buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");

  char *expected =
      "abcdefghijklmnopqrstuvwxyz12345678910abcdefghijklmnopqrstuvwxyz";

  is(buf->state, expected, "buffer holds all appended characters in order");
  int expected_len = strlen(expected);
  ok(buf->len == strlen(expected), "buffer's length is %d", expected_len);

  buffer_free(buf);
}

void test_buffer_append_with() {
  Buffer *buf = buffer_init(NULL);

  buffer_append_with(buf, "abcdefghijklmnopqrstuvwxyz", 10);
  buffer_append_with(buf, "12345678910", 5);
  buffer_append_with(buf, "abcdefghijklmnopqrstuvwxyz", 10);

  char *expected = "abcdefghij12345abcdefghij";

  is(buf->state, expected, "buffer contains all appended characters in order");
  int expected_len = strlen(expected);
  ok(buf->len == expected_len, "buffer's length is %d", expected_len);

  // buffer_free(buf);
}

void test_buffer_concat() {
  Buffer *buf_a = buffer_init("test");
  Buffer *buf_b = buffer_init("string");
  Buffer *buf_c = buffer_concat(buf_a, buf_b);

  printf("state a %s len a %d state b %s len b %d state c %s c len %d\n",
         buf_a->state, strlen(buf_a->state), buf_b->state, strlen(buf_b->state),
         buf_c->state, strlen(buf_c->state));
  // is(buf_c->state, "teststring", "concatenates the two buffers' states");
  // ok(buf_c->len == strlen("teststring"), "has the correct length");

  // buffer_free(buf_a);
  // buffer_free(buf_b);
  // buffer_free(buf_c);
}

void test_buffer_concat_on_null() {
  Buffer *buf_a = buffer_init(NULL);
  Buffer *buf_b = buffer_init(NULL);

  Buffer *buf_c = buffer_concat(buf_a, buf_b);

  is(buf_c, NULL, "returns NULL");

  buffer_free(buf_a);
  buffer_free(buf_b);
}

void test_buffer_free() {
  Buffer *buf = buffer_init(NULL);

  lives_ok({ buffer_free(buf); }, "frees the buffer's heap memory");
}

void test_buffer_free_nonnull() {
  Buffer *buf = buffer_init("test");

  lives_ok({ buffer_free(buf); }, "frees the buffer's memory");
}

int main(int argc, char *argv[]) {
  // plan(11);

  test_buffer_free();
  test_buffer_free_nonnull();
  test_buffer_init();
  test_buffer_init_with_initial();
  test_buffer_append();
  // test_buffer_append_with();
  test_buffer_concat();
  test_buffer_concat_on_null();

  done_testing();
}

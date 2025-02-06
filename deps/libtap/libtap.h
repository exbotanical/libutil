#ifndef LIBTAP_H
#define LIBTAP_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: parser for gh actions, etc
unsigned int __tap_ok(unsigned int ok, const char* fn_name, const char* file,
                      const unsigned int line, const char* fmt, ...);
void __tap_skip(unsigned int num_skips, const char* msg);
int __tap_write_shared_mem(int status);

#define __tap_lives_or_dies(wants_death, code, ...)                    \
  do {                                                                 \
    /* set shared memory to 1 */                                       \
    __tap_write_shared_mem(1);                                         \
                                                                       \
    pid_t pid = fork();                                                \
    switch (pid) {                                                     \
      case -1: {                                                       \
        perror("fork");                                                \
        exit(EXIT_FAILURE);                                            \
      }                                                                \
      case 0: {                                                        \
        close(STDOUT_FILENO);                                          \
        close(STDERR_FILENO);                                          \
        /* execute test code, then set shared memory to zero */        \
        code __tap_write_shared_mem(0);                                \
        exit(EXIT_SUCCESS);                                            \
      }                                                                \
    }                                                                  \
                                                                       \
    if (waitpid(pid, NULL, 0) < 0) {                                   \
      perror("waitpid");                                               \
      exit(EXIT_FAILURE);                                              \
    }                                                                  \
    /* grab prev value (and reset) - if 0, code succeeded */           \
    int test_died = __tap_write_shared_mem(0);                         \
    if (!test_died) {                                                  \
      code                                                             \
    }                                                                  \
    __tap_ok(wants_death ? test_died : !test_died, __func__, __FILE__, \
             __LINE__, __VA_ARGS__);                                   \
  } while (0)

/* Begin public APIs */

void todo_start(const char* fmt, ...);
void todo_end(void);
void diag(const char* fmt, ...);
void plan(unsigned int num_tests);
unsigned int exit_status(void);
unsigned int bail_out(const char* fmt, ...);

#define done_testing() return exit_status()
// clang-format off
#define ok(test, ...)      __tap_ok(test ? 1 : 0, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define eq_num(a, b, ...)  __tap_ok(a == b ? 1 : 0, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define neq_num(a, b, ...) __tap_ok(a != b ? 1 : 0, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define eq_str(a, b, ...)  __tap_ok(strcmp(a, b) == 0 ? 1 : 0, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define neq_str(a, b, ...) __tap_ok(strcmp(a, b) == 0 ? 0 : 1, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define eq_null(a, ...)    __tap_ok(a == NULL, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define neq_null(a, ...)   __tap_ok(a != NULL, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define eq_true(a, ...)    __tap_ok(a == true, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define eq_false(a, ...)    __tap_ok(a == false, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define lives(...)         __tap_lives_or_dies(0, __VA_ARGS__)
#define dies(...)          __tap_lives_or_dies(1, __VA_ARGS__)
#define pass(...)          ok(1, __VA_ARGS__)
#define fail(...)          ok(0, __VA_ARGS__)

#define skip_start(cond, num_skips, msg) do { if (cond) { __tap_skip(num_skips, msg); break; }
#define skip_end() } while (0)
#define skip(test, msg) __tap_skip(1, msg);

#ifdef TAP_WANT_PCRE

unsigned int
__tap_match (
  const char* string,
  const char* pattern,
  bool want_match,
  const char* fn_name,
  const char* file,
  const unsigned int line,
  const char* fmt,
  ...
);

#define match_str(string, pattern, ...)  __tap_match(string, pattern, true, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define nomatch_str(string, pattern, ...)  __tap_match(string, pattern, false, __func__, __FILE__, __LINE__, __VA_ARGS__)

#endif /* TAP_WANT_PCRE */

// clang-format on
#ifdef __cplusplus
}
#endif

#endif /* LIBTAP_H */

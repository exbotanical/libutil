#ifndef LIBTAP_H
#define LIBTAP_H

#include <stdarg.h>
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

static char* __s_fmt__(char* fmt, ...) {
  va_list args, args_cp;
  va_start(args, fmt);
  va_copy(args_cp, args);

  // Pass length of zero first to determine number of bytes needed
  unsigned int n = vsnprintf(NULL, 0, fmt, args) + 1;
  char* buf = (char*)malloc(n);
  if (!buf) {
    return NULL;
  }

  vsnprintf(buf, n, fmt, args_cp);

  va_end(args);
  va_end(args_cp);

  return buf;
}

unsigned int __ok(unsigned int ok, const char* fn_name, const char* file,
                  const unsigned int line, char* msg);

void __skip(unsigned int num_skips, char* msg);

int __write_shared_mem(int status);

void todo_start(const char* fmt, ...);

void todo_end(void);

void diag(const char* fmt, ...);

void plan(unsigned int num_tests);

unsigned int exit_status(void);

unsigned int bail_out(const char* fmt, ...);

#define ok(test, ...) \
  __ok(test ? 1 : 0, __func__, __FILE__, __LINE__, __s_fmt__(__VA_ARGS__))

#define is(actual, expected, ...)                 \
  __ok(!(actual == expected ? 0                   \
         : !actual          ? -1                  \
         : !expected        ? 1                   \
                            : strcmp(actual, expected)), \
       __func__, __FILE__, __LINE__, __s_fmt__(__VA_ARGS__));

#define skip_start(cond, num_skips, ...)         \
  do {                                           \
    if (cond) {                                  \
      __skip(num_skips, __s_fmt__(__VA_ARGS__)); \
      break;                                     \
    }

#define skip_end() \
  }                \
  while (0)

#define skip(test, ...) __skip(1, __s_fmt__(__VA_ARGS__));

#define done_testing() return exit_status()

#define lives(...) _lives_or_dies(0, __VA_ARGS__)
#define dies(...) _lives_or_dies(1, __VA_ARGS__)

#define _lives_or_dies(wants_death, code, ...)                               \
  do {                                                                       \
    /* set shared memory to 1 */                                             \
    __write_shared_mem(1);                                                   \
                                                                             \
    int pid = fork();                                                        \
    switch (pid) {                                                           \
      case -1: {                                                             \
        perror("fork");                                                      \
        exit(EXIT_FAILURE);                                                  \
      }                                                                      \
      case 0: {                                                              \
        close(STDOUT_FILENO);                                                \
        close(STDERR_FILENO);                                                \
        /* execute test code, then set shared memory to zero */              \
        code __write_shared_mem(0);                                          \
        exit(EXIT_SUCCESS);                                                  \
      }                                                                      \
    }                                                                        \
                                                                             \
    if (waitpid(pid, NULL, 0) < 0) {                                         \
      perror("waitpid");                                                     \
      exit(EXIT_FAILURE);                                                    \
    }                                                                        \
    /* grab prev value (and reset) - if 0, code succeeded */                 \
    int test_died = __write_shared_mem(0);                                   \
    if (!test_died) {                                                        \
      code                                                                   \
    }                                                                        \
    __ok(wants_death ? test_died : !test_died, __func__, __FILE__, __LINE__, \
         __s_fmt__(__VA_ARGS__));                                            \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif /* LIBTAP_H */

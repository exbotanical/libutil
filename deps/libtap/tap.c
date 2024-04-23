#define _DEFAULT_SOURCE 1  // For vasprintf
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "libtap.h"

#if defined __APPLE__ || defined BSD
#  define MAP_ANONYMOUS MAP_ANON
#endif

#ifdef WANT_PTHREAD
#  include <pthread.h>
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#  define lock()   pthread_mutex_lock(&mutex)
#  define unlock() pthread_mutex_unlock(&mutex)
#else
#  define lock()
#  define unlock()
#endif

#define with_lock(code) \
  lock();               \
  code unlock();

#define TAP_FAILURE_EXIT_STATUS 255

static unsigned int has_plan          = 0;
static pid_t        test_runner_pid   = 0;
static unsigned int test_died         = 0;
static unsigned int num_planned_tests = 0;
static unsigned int num_ran_tests     = 0;
static unsigned int num_failed_tests  = 0;
static unsigned int is_todo_block     = 0;
static char*        todo_msg          = NULL;

noreturn void
panic (const unsigned int errcode, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  exit(errcode);
}

static void
diagv (const char* fmt, va_list ap) {
  fprintf(stdout, "# ");
  vfprintf(stdout, fmt, ap);
  fprintf(stdout, "\n");
}

static void
cleanup (void) {
  // Fast exit if we've forked
  if (getpid() != test_runner_pid) {
    return;
  }

  lock();

  if (!has_plan) {
    diag("Test died before any output could be written");

    unlock();

    return;
  }

  if (test_died) {
    diag("Test died after test %d", num_ran_tests);

    unlock();

    return;
  }

  if (has_plan) {
    if (num_planned_tests != num_ran_tests) {
      diag(
        "Planned %d %s but ran %d",
        num_planned_tests,
        num_planned_tests == 1 ? "test" : "tests",
        num_ran_tests
      );
    }
  }

  if (num_failed_tests) {
    diag("Failed %d tests of %d", num_failed_tests, num_ran_tests);
  }

  unlock();
}

unsigned int
__ok (
  unsigned int       ok,
  const char*        fn_name,
  const char*        file,
  const unsigned int line,
  char*              msg
) {
  lock();

  num_ran_tests++;

  if (!ok) {
    num_failed_tests++;
    printf("not ");
  }

  fprintf(stdout, "ok %d - ", num_ran_tests);

  // See "Escaping" - https://testanything.org/tap-version-14-specification.html
  char* c;
  flockfile(stdout);
  for (c = msg; *c != '\0'; c++) {
    if (*c == '#') {
      fputc('\\', stdout);
    }
    fputc((int)*c, stdout);
  }
  funlockfile(stdout);

  if (is_todo_block) {
    fprintf(stdout, " # TODO %s", todo_msg);
  }
  fprintf(stdout, "\n");
  free(msg);

  if (!ok) {
    diag(
      "\tFailed %stest (%s:%s at line %d)",
      is_todo_block ? "(TODO)" : "",
      file,
      fn_name,
      line
    );
  }

  unlock();

  return ok ? 1 : 0;
}

void
__skip (unsigned int num_skips, char* msg) {
  with_lock({
    while (num_skips--) {
      fprintf(stdout, "ok %d # SKIP %s", ++num_ran_tests, msg);
      fprintf(stdout, "\n");
    }
    free(msg);
  });
}

int
__write_shared_mem (int status) {
  static int* test_died = NULL;
  int         prev;

  if (!test_died) {
    test_died = mmap(
      0,
      sizeof(int),
      PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS,
      -1,
      0
    );
    *test_died = 0;
  }

  prev       = *test_died;
  *test_died = status;
  return prev;
}

void
todo_start (const char* fmt, ...) {
  va_list ap;

  with_lock({
    va_start(ap, fmt);
    if (vasprintf(&todo_msg, fmt, ap) < 0) {
      todo_msg = NULL;
    }
    va_end(ap);

    is_todo_block = 1;
  });
}

void
todo_end (void) {
  with_lock({
    is_todo_block = 0;
    free(todo_msg);
    todo_msg = NULL;
  });
}

void
diag (const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  diagv(fmt, ap);
  va_end(ap);
}

void
plan (unsigned int num_ran_tests) {
  lock();

  static unsigned int singleton = 0;
  if (!singleton) {
    test_runner_pid = getpid();
    atexit(cleanup);
  }

  singleton = 1;

  if (has_plan) {
    test_died = 1;

    unlock();

    panic(TAP_FAILURE_EXIT_STATUS, "plan was called twice\n");
  }

  if (num_ran_tests == 0) {
    test_died = 1;

    unlock();

    panic(TAP_FAILURE_EXIT_STATUS, "no tests planned\n");
  }

  has_plan          = 1;

  num_planned_tests = num_ran_tests;
  fprintf(stdout, "1..%d\n", num_planned_tests);

  unlock();
}

unsigned int
exit_status (void) {
  unsigned int retval;

  with_lock({
    if (num_planned_tests < num_ran_tests) {
      retval = num_ran_tests - num_planned_tests;
    } else {
      retval = num_failed_tests + (num_planned_tests - num_ran_tests);
    }

    if (retval > TAP_FAILURE_EXIT_STATUS) {
      retval = TAP_FAILURE_EXIT_STATUS;
    }
  });

  return retval;
}

unsigned int
bail_out (const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  fprintf(stdout, "Bail out!  ");
  vprintf(fmt, args);
  printf("\n");
  va_end(args);

  exit(TAP_FAILURE_EXIT_STATUS);

  return EXIT_SUCCESS;
}

#include <stdio.h>

#include "libutil.h"
#include "tap.c/tap.h"

int main(int argc, char const *argv[]) {
  printf(fmt_str("%s %d\n", "str", 111));
  cmp_ok(1, "==", 1);
  return 0;
}

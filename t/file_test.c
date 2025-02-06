#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tests.h"

#define TMP_FILE "/tmp/fileXXXXXX"
#define TMP_DIR "/tmp/dirXXXXXX"
#define TMP_SYMLINK "/tmp/symlinkXXXXXX"

static void test_file_is_pointer_valid(void) {
  eq_true(file_is_pointer("."), "'.' should be considered a pointer");
  eq_true(file_is_pointer(".."), "'..' should be considered a pointer");
}

static void test_file_is_pointer_invalid(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  eq_false(file_is_pointer(tmp_file),
           "'file.txt' should not be considered a pointer");

  remove(tmp_file);
}

static void test_file_exists_existing_file(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  eq_true(file_exists(tmp_file), "file should exist");

  remove(tmp_file);
}

static void test_file_exists_nonexistent_file(void) {
  eq_false(file_exists("nonexistent_file.txt"),
           "non-existent file should return false");
}

static void test_file_is_symlink_regular_file(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  eq_false(file_is_symlink(tmp_file), "regular file should not be a symlink");

  remove(tmp_file);
}

static void test_file_is_symlink_symlink(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  char tmp_symlink[] = TMP_SYMLINK;
  symlink(tmp_file, tmp_symlink);
  eq_true(file_is_symlink(tmp_symlink), "should be recognized as a symlink");

  remove(tmp_file);
  remove(tmp_symlink);
}

static void test_file_is_executable_executable_file(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  FILE* file = fdopen(fd, "w");
  fprintf(file, "#!/bin/bash\necho Hello World\n");
  fclose(file);

  chmod(tmp_file, S_IRUSR | S_IWUSR | S_IXUSR);
  eq_true(file_is_executable(tmp_file), "should be recognized as executable");

  remove(tmp_file);
}

static void test_file_is_executable_non_executable_file(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  eq_false(file_is_executable(tmp_file), "should not be executable");

  remove(tmp_file);
}

static void test_file_is_directory_valid_directory(void) {
  char tmp_dir[] = TMP_DIR;
  mkdtemp(tmp_dir);

  eq_true(file_is_directory(tmp_dir), "should be recognized as a directory");

  rmdir(tmp_dir);
}

static void test_file_is_directory_non_directory(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  eq_false(file_is_directory(tmp_file),
           "should not be recognized as a directory");

  remove(tmp_file);
}

static void test_file_is_symlinked_to_valid_symlink(void) {
  char tmp_file[] = TMP_FILE;
  int fd = mkstemp(tmp_file);
  close(fd);

  char tmp_symlink[] = TMP_SYMLINK;
  symlink(tmp_file, tmp_symlink);

  eq_true(file_is_symlinked_to(tmp_file, tmp_symlink),
          "symlink points to file");

  remove(tmp_file);
  remove(tmp_symlink);
}

static void test_file_is_symlinked_to_invalid_symlink(void) {
  char tmp_symlink[] = TMP_SYMLINK;
  symlink("nonexistent_file.txt", tmp_symlink);

  eq_false(file_is_symlinked_to(tmp_symlink, "test_file_is_symlinked_to.txt"),
           "non-symlink should return false");

  remove(tmp_symlink);
}

void run_file_tests(void) {
  test_file_is_pointer_valid();
  test_file_is_pointer_invalid();
  test_file_exists_existing_file();
  test_file_exists_nonexistent_file();
  test_file_is_symlink_regular_file();
  test_file_is_symlink_symlink();
  test_file_is_executable_executable_file();
  test_file_is_executable_non_executable_file();
  test_file_is_directory_valid_directory();
  test_file_is_directory_non_directory();
  test_file_is_symlinked_to_valid_symlink();
  test_file_is_symlinked_to_invalid_symlink();
}

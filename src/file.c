#define __USE_XOPEN_EXTENDED 1
#include <sys/stat.h>
#include <unistd.h>

#include "libutil.h"

bool file_is_pointer(const char* filepath) {
  return s_equals(filepath, ".") || s_equals(filepath, "..");
}

bool file_exists(const char* filepath) {
  struct stat buf;
  return stat(filepath, &buf) == 0;
}

bool file_is_symlink(const char* filepath) {
  struct stat st;
  return (lstat(filepath, &st) == 0 && S_ISLNK(st.st_mode));
}

bool file_is_executable(const char* filepath) {
  return access(filepath, X_OK) == 0;
}

bool file_is_directory(const char* filepath) {
  struct stat st;
  return (stat(filepath, &st) == 0) && S_ISDIR(st.st_mode);
}

bool file_is_symlinked_to(const char* filepath, const char* link) {
  char resolved_path[1024];
  ssize_t len = readlink(link, resolved_path, sizeof(resolved_path) - 1);

  if (len == -1) {
    return false;
  }

  resolved_path[len] = '\0';
  return (access(filepath, F_OK) == 0) && (s_equals(resolved_path, filepath));
}

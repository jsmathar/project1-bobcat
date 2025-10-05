#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 8192  // 8kb buffer for reading and writing

int main(int argc, char* argv[]) {
  char buffer[BUFFER_SIZE];
  bool had_error = false;
  ssize_t bytes_read;
  // if no arguments, read from standard input, as per instructions
  if (argc == 1) {
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
      if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
        warnx("write failed");
        return 1;
      }
    }
    if (bytes_read < 0) {
      warnx("read failed");
      return 1;
    }
    return 0;
  }
  // if there are arguments, loop
  for (int i = 1; i < argc; i++) {
    char* current_arg = argv[i];

    // our descriptor is the dash '-',and that indicates stadard in
    int file_descriptor;
    if (strcmp(current_arg, "-") == 0) {
      file_descriptor = STDIN_FILENO;
    } else {
      file_descriptor = open(current_arg, O_RDONLY);  // open in read-only
      if (file_descriptor < 0) {
        warnx("%s", current_arg);
        had_error = true;
        continue;
      }
    }

    // copy from file (or stdin) to stdout
    // same as above, but stdout
    while ((bytes_read = read(file_descriptor, buffer, BUFFER_SIZE)) > 0) {
      if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
        warnx("write failed");
        had_error = true;
        break;
      }
    }
    if (bytes_read < 0) {
      if (file_descriptor == STDIN_FILENO && errno == EISDIR)
        warnx("bobcat: -: Is a directory");
      else if (errno == EISDIR)
        warnx("bobcat: %s: Is a directory", current_arg);
      else
        warnx("bobcat: read failed");
      had_error = true;
    }

    // close the file if it was actually opened (not stdin)
    // and so no memory is leaked
    if (file_descriptor != STDIN_FILENO) {
      if (close(file_descriptor) < 0) {
        warnx("%s", current_arg);
        had_error = true;
      }
    }
  }

  return had_error ? 1 : 0;
}

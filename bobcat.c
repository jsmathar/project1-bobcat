#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define BUFFER_SIZE 8192  // 8kb buffer for reading and writing

int main(int argc, char* argv[]) {
  char buffer[BUFFER_SIZE];
  bool had_error = false;
  // if no arguments, read from standard input, as per instructions
  if (argc == 1) {
    ssize_t bytes_read;
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
      if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
        warn("write failed");
        return 1;
      }
    }
    if (bytes_read < 0) {
      warn("read failed");
      return 1;
    }
    return 0;
  }
  // if there are arguments, loop
  for (int i = 1; i < argc; i++) {
    char* current_arg = argv[i];
    // our descriptor is the dash '-',and that
    // indicates stadard in
    int file_descriptor;
    if (strcmp(current_arg, "-") == 0) {
      file_descriptor = STDIN_FILENO;
    } else {
      file_descriptor = open(current_arg, O_RDONLY);  // open in read-only
      if (file_descriptor < 0) {
        warn("%s", current_arg);
        had_error = true;
        continue;
      }
    }
    // copy from file (or stdin) to stdout
    // same as above, but stdout
    ssize_t bytes_read;
    while ((bytes_read = read(file_descriptor, buffer, BUFFER_SIZE)) > 0) {
      if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
        warn("write failed");
        had_error = true;
        break;
      }
    }
    if (bytes_read < 0) {
      warn("read failed");
      had_error = true;
    }
    // close the file if it was actually opened (not stdin) // and so no
    // memory is leaked
    if (file_descriptor != STDIN_FILENO) {
      if (close(file_descriptor) < 0) {
        warn("%s", current_arg);
        had_error = true;
      }
    }
  }
  return had_error ? 1 : 0;
}

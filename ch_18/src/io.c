#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t read_file(char const *f_name, CharArray *out, size_t max_n_str) {
  FILE *f = fopen(f_name, "r");
  if (!f) {
    perror("fopen failed");
    return 0;
  }
  char buf[MAX_STR_LINE];
  size_t pos = 0;
  while (pos < max_n_str && fgets(buf, MAX_STR_LINE, f)) {
    if (buf[MAX_STR_LINE - 1] != '\n' && buf[MAX_STR_LINE - 1] != '\0' &&
        buf[MAX_STR_LINE - 1] != '\r') {
      perror("string is too long!");
      return 0;
    } else {
      size_t str_len = strlen(buf);
      if (buf[str_len - 1] == '\r' || buf[str_len - 1] == '\n')
        buf[str_len - 1] = '\0';
      if (str_len > 1 && (buf[str_len - 2] == '\r' || buf[str_len - 2] == '\n'))
        buf[str_len - 2] = '\0';
      memcpy(out[pos].str, buf, strlen(buf));
    }
    pos++;
  }
  fclose(f);
  return pos;
}

#ifdef DEBUG

int main(int argc, char *argv[argc + 1]) {
  char *f_name = argv[1];
  const size_t max_n_str = 2100;
  const size_t max_n = 256;
  char **str_arr = calloc(sizeof(char *), max_n_str);
  for (size_t i = 0; i < max_n_str; ++i) {
    str_arr[i] = calloc(sizeof(char), max_n);
  }

  size_t n_str = read_file(f_name, str_arr, max_n_str, max_n);
  printf("%zu\n", n_str);
  for (size_t i = 0; i < n_str; ++i) {
    printf("%s\n", str_arr[i]);
    free(str_arr[i]);
  }
  free(str_arr);
}

#endif
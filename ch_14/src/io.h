#ifndef __IO_H_
#define __IO_H_

#include "utils.h"
#include <stddef.h>

size_t read_file(char const *f_name, CharArray* out, size_t max_n_str);

#endif
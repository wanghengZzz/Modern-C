#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdio.h>
#define CHECK_ERR(cond, err)        \
  do {                              \
    if (!(cond)) {                  \
      perror("" #err " :falied\n"); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (0);

#endif
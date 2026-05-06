#include "funcs.h"

double get_factorial(int x) {
  long long init = 1;
  do {
    init *= x;
  } while (--x);
  return 1.0 / init;
}
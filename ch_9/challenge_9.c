#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define nullptr ((void *)0)
int check_strtoul(char *endptr)
{
    if (errno == ERANGE)
        puts("Numerical overflow\n");
    else if (!endptr || *endptr != '\0')
        printf("Can't not parse charater %c\n", *endptr);
    else
        return 1;
    return 0;
}

#define CHECK_ERR(cond, err)        \
  do {                              \
    if (!(cond)) {                  \
      perror("" #err " :falied\n"); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (0);

size_t find_prime(size_t n)
{
    size_t prime_arr[n + 1];
    memset(prime_arr, 0, (n + 1) * sizeof(size_t));
    printf("%zu's prime factors: ", n);
    for (size_t i = 2; i <= n; ++i) {
        if (!prime_arr[i]) {
            prime_arr[i] = 1;
            if (!(n % i))
                printf("%zu ", i);
            for (size_t j = i << 1; j <= n; j += i)
                prime_arr[j] = 1;
        }
    }
    putchar('\n');
}

int main(int argc, char *argv[argc + 1])
{
    for (size_t i = 1; i < argc; ++i) {
        char *endptr = nullptr;
        size_t num = strtoul(argv[i], &endptr, 0);
        CHECK_ERR(check_strtoul(endptr), "invalid input type");
        find_prime(num);
    }

    return 0;
}
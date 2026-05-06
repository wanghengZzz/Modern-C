#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    References:
        Computing Pi in C (https://crypto.stanford.edu/pbc/notes/pi/code.html)
        Euler's convergence improvement (https://mathworld.wolfram.com/PiFormulas.html)
 */

int main(int argc, char *argv[argc + 1])
{
    long num_digits = 14 * (argc > 1 ? strtol(argv[1], 0, 0) : 200);
    
    
    long long r[num_digits + 1];
    long long i, k;
    long long b, d;
    long long c = 0;
    for (i = 0; i < num_digits; ++i)
        r[i] = 2000;
    
    r[i] = 0;
    for (k = num_digits; k > 0; k -= 14) {
        d = 0;
        i = k;

        /*
            ex: 
              2 * (1 + (1/3) * (1 + (2/5) * (1 + (3/7)))) = 
              (2 + (1/3) * (2 + (2/5) * (2 + 2 * (3/7))))
        */

        for (;;) {
            d += r[i] * 10000; // 2^14 > 10^4,  
            b = 2 * i - 1;
            r[i] = d % b;
            d /= b;
            i--;
            if (!i) break;
            d *= i;
        }

        printf("%.4lld", c + d / 10000);
        c = d % 10000;
    }
    printf("\n");

    return EXIT_SUCCESS;
}
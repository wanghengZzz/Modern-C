## Modern C Challenges: Complete Solutions & Implementations

Comprehensive solutions for all challenge problems in the book **"Modern C" by Jens Gustedt** (2nd Edition, 2019).

## Brief Indroductions and Related Topics of Used Knowledges for the Challenges

* `ch1-Sequential sorting algorithms`
  - Merge sort with recursion
  - Quick sort with recursion
* `ch2-Numerical derivatives (only for real number)`
  - Taylor series
  - Trig function
  - Central difference formula 
* `ch3-Compute the N first decimal place of PI`
  - Euler's convergence improvement
* `ch4-Union & Find`
  - Disjoint set
* `ch5-Numerical derivatives but extended to complex numbers`
  - Cauchy-Riemann equations
* `ch6-Matrix inversion`
  - Gaussian elimination
  - Linear algebra
* `ch7,8-Impementations of classical algorithms and data structures`
  - BFS (breadth-first search)
  - Connected Components
  - Minimum spanning tree
  - Shortest path problems
  - heap, deque, disjoint set
* `ch9-Smallest prime factor`
* `ch10-Performace comparison of sorting algorithms`
* `ch11-Image segmentation`
  - disjoint set
* `ch12-Text processing in C`
* `ch13-Generic derivative, roots, zeros and complex zeros of polynomials`
  - Newton's method
  - linear algebra
    - QR decomposition
    - Wilkinson shift
    - Givens rotation
    - Hessenberg matrix
    - Eigenvalues
* `ch14-Generic sorting and performance comparison to qsort`
* `ch15-Implementation of Regular expression matching Parser`
  - AST (Abstract Syntax Tree)
  - Recursive Descent Parser
  - Non-deterministic Finite Automaton
* `ch16-Text processing in streams and error handling`
* `ch17-Implementation of Regular expression matching Parser but with multibytes characters`
* `ch18-Parallel sorting with threads`

## Quick Start

``` shell

cd ch_xx
make
make run
```
## Development Tools
- Check for memory leaks and errors using Valgrind `make valgrind`
- Auto-format source files via clang-format `make format`
- Remove binaries and intermediate object files `make clean`
# Program 1 – CS 344

I wrote bash shell scripts to compute matrix operations. The purpose is to get me familiar with the Unix shell, shell programming, Unix utilities, standard input, output, and error, pipelines, process ids, exit values, and signals (at a basic level).

My script is called simply "matrix".

## Overview

In this assignment, you will write a bash shell script that calculates basic matrix operations using input from either a file or stdin. The input will be whole number values separated by tabs into a rectangular matrix. Your script should be able to print the dimensions of a matrix, transpose a matrix, calculate the mean vector of a matrix, add two matrices, and multiply two matrices.

You will be using bash builtins and Unix utilities to complete the assignment. Some commands to read up on are `while`, `cat`, `read`, `expr`, `cut`, `head`, `tail`, `wc`, and `sort`.

Your script must be called simply "matrix". The general format of the matrix command is:

```
matrix OPERATION [ARGUMENT]...
```

Refer to man(1) (You can do this with the command man1manman1man) for an explanation of the conventional notation regarding command syntax, to understand the line above. Note that many terminals render italic font style as an underline:

```
matrix OPERATION [ARGUMENT]...
```

## Specifications

Your program must perform the following operations: dims, transpose, mean, add, and multiply. Usage is as follows:

```
matrix dims [MATRIX]
matrix transpose [MATRIX]
matrix mean [MATRIX]
matrix add MATRIX_LEFT MATRIX_RIGHT
matrix multiply MATRIX_LEFT MATRIX_RIGHT
```

The dims, transpose, and mean operations should either perform their respective operations on the file named *MATRIX*, or on a matrix provided via stdin. The add and multiply operations do not need to process input via stdin.

- dims should print the dimensions of the matrix as the number of rows, followed by a space, then the number of columns.
- transpose should reflect the elements of the matrix along the main diagonal. Thus, an MxN matrix will become an NxM matrix and the values along the main diagonal will remain unchanged.
- mean should take an MxN matrix and return an 1xN row vector, where the first element is the mean of column one, the second element is the mean of column two, and so on.
- add should take two MxN matrices and add them together element-wise to produce an MxN matrix. add should return an error if the matrices do not have the same dimensions.
- multiply should take an MxN and NxP matrix and produce an MxP matrix. Note that, unlike addition, matrix multiplication is not commutative. That is A*B != B*A.

Here is a brief example of what the output should look like.

```
$ cat m1
1	2	3	4
5	6	7	8
$ cat m2
1	2
3	4
5	6
7	8
$ ./matrix dims m1
2 4
$ cat m2 | ./matrix dims
4 2
$ ./matrix add m1 m1
2	4	6	8
10	12	14	16
$ ./matrix add m2 m2
2	4
6	8
10	12
14	16
$ ./matrix mean m1
3	4	5	6
$ ./matrix transpose m1
1	5
2	6
3	7
4	8
$ ./matrix multiply m1 m2
50	60
114	140
```

You must check for the right number and format of arguments to matrix. This means that, for example, you must check that a given input file is readable, before attempting to read it. You are not required to test if the input file *itself* is valid. In other words, the behavior of matrix is undefined when the matrix input is not a valid matrix. for the purposes of this assignment, a valid matrix is a tab-delimited table containing at least one element, where each element is a signed integer, every entry is defined, and the table is rectangular.

The following are examples of invalid matrices. Our grading script (see below) does not send these as *input* into your matrix program. Similarly, you aren't allowed to send matrices with these characteristics as *output* from your script either, and we will test your output to make sure it doesn't. If any of these are found in your output, the grading script will deduct points:

- An empty matrix.
- A matrix where the final entry on a row is followed by a tab character.
- A matrix with empty lines.
- A matrix with any element that is blank or not an integer.

Here is a valid matrix file, m1:

```
$ cat m1
8	5	6
3	2	2
1	6	7
5	0	7
2	2	4
$ cat -A m1   # The '-A' flag shows tabs as '^I' and newlines as '$'. This is a good way to check correctness.
8^I5^I6$
3^I2^I2$
1^I6^I7$
5^I0^I7$
2^I2^I4$
$ 
```

If the inputs are valid -- your program should output only to stdout, and nothing to stderr. The return value should be 0.

If the inputs are invalid -- your program should output only to stderr, and nothing to stdout. The return value should be any number except 0. The error message you print is up to you; you will receive points as long as you print *something* to stderr and return a non-zero value.

Your program will be tested with matrices up to dimension 100 x 100.
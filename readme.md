# Simple Bignum
**A simple Arbitrary Precision Arithmetic in C**


### Description
Arbitrary-Precision arithmetic, also known as "bignum" or simply "long arithmetic" is a set of data structures and algorithms which allows to process much greater numbers than can be fit in standard data types. 

This project was an assignment for my course COP290 Design Practices at IIT Delhi in which I had to implement "bignum", so I added this on GitHub.

Program supports the following:

- addition, subtraction, multiplication and division.
- Square Root, abs, power(exp)

It also works for negative numbers.

### Running the programming
```
$ make
$ ./mainfile input.txt output.txt
$ make clean
```

### input.txt and output.txt format

here input.txt contains instructions in format 
```
ADD, SUB, MUL, DIV, ABS, POW, SQRT
```
example input.txt file
```c
POW 5678 1000
POW 1.34567 -3
DIV 4.500000000000000000 0.00004500000
```
then output.txt file will be
```c
156815...29376
0.41037820598540834667
100000
```

### function definations

- bignum structure

    ```c
    struct bignum_struct {
        char *revdigits; // digits are stored in reverse order
        int sign;
        int size;
        int decimal;
    } bignum_default = {NULL, PLUS, 0, 0};

    typedef struct bignum_struct bignum;
    ```

- declaring the variables

    ```c
    bignum b1 = bignum_default;
    initialize_bignum(&b1, "434343.4343");
    bignum b2 = bignum_default;
    initialize_bignum(&b2, "-90.4343");
    bignum ans = bignum_default;
    ```
- sadd stores the **ADD** result in ans. s here means it is signed and u means it is unsigned

    ```c
    sadd(&b1, &b2, &ans);
    ```

- ssub stores the **SUB** result in ans.

    ```c
    ssub(&b1, &b2, &ans);
    ```

- smultiply stores the **MULT** result in ans.

    ```c
    smultiply(&b1, &b2, &ans);
    ```

- sdivide stores the **DIV** result in ans.
    here *int precision = 20*, *int roundOff = 0*
    ```c
    sdivide(&b1, &b2, &ans, 20, 0);
    ```
    precision is the decimal digits upto which you should get the result and roundOff means that whether you should round off the least significant digit which getting ans truncated upto precision.

- makes store **ABS** value of b1 in b1 itself.

    ```c
    absolute(&b1);
    ```

- spower store **POW** value in the ans

    ```c
    spower(&b1, &b2, &ans, precision: 20, roundOff: 0);
    ```
    Here also precision and roundOff means same as of division

- usquareRoot store **SQRT** value in the ans. u is unsigned here. i.e. ignores the sign of variables

    ```c
    usquareRoot(&b1, &ans, precision: 20, roundOff: 0);
    ```
    Here also precision and roundOff means same as of division
    
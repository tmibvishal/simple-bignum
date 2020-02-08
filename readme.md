# Assignment 3

### Vishal Singh (2018CS50426)

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
```
POW 5678 1000
POW 1.34567 -3
DIV 4.500000000000000000 0.00004500000
```
then output.txt file will be
```
156815...29376
0.41037820598540834667
100000
```

### function definations

- bignum structure

    ```
    struct bignum_struct {
        char *revdigits; // digits are stored in reverse order
        int sign;
        int size;
        int decimal;
    } bignum_default = {NULL, PLUS, 0, 0};

    typedef struct bignum_struct bignum;
    ```

- declaring the variables

    ```
    bignum b1 = bignum_default;
    initialize_bignum(&b1, "434343.4343");
    bignum b2 = bignum_default;
    initialize_bignum(&b2, "-90.4343");
    bignum ans = bignum_default;
    ```
- sadd stores the **ADD** result in ans. s here means it is signed and u means it is unsigned

    ```
    sadd(&b1, &b2, &ans);
    ```

- ssub stores the **SUB** result in ans.

    ```
    ssub(&b1, &b2, &ans);
    ```

- smultiply stores the **MULT** result in ans.

    ```
    smultiply(&b1, &b2, &ans);
    ```

- sdivide stores the **DIV** result in ans.
    here *int precision = 20*, *int roundOff = 0*
    ```
    sdivide(&b1, &b2, &ans, 20, 0);
    ```
    precision is the decimal digits upto which you should get the result and roundOff means that whether you should round off the least significant digit which getting ans truncated upto precision.

- makes store **ABS** value of b1 in b1 itself.

    ```
    absolute(&b1);
    ```

- spower store **POW** value in the ans

    ```
    spower(&b1, &b2, &ans, precision: 20, roundOff: 0);
    ```
    Here also precision and roundOff means same as of division

- usquareRoot store **SQRT** value in the ans. u is unsigned here. i.e. ignores the sign of variables

    ```
    usquareRoot(&b1, &ans, precision: 20, roundOff: 0);
    ```
    Here also precision and roundOff means same as of division
    
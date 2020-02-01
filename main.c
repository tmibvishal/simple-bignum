#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define PLUS 0
#define MINUS 1

/*

Notes:
 free(malloc) memories
 use error: no space on mallow ptr null
 not print starting 0 or trailing 0

*/
typedef struct {
    char *revdigits; // digits are stored in reverse order
    int sign;
    int size;
    int decimal;
} bignum;

int max(int size, int size1);

void uadd(bignum *b1, bignum *b2, bignum *ans, int c, int initialize) {
    if (initialize) {
        ans->decimal = max(b1->decimal, b2->decimal);
        int length = max(b1->size - b1->decimal,b2->size - b2->decimal) + ans->decimal + 1;
        ans->size = length;
        ans->sign = PLUS;
        ans->revdigits = malloc(length * sizeof(char));
    }

    int skip1 = 0;
    int skip2 = 0;

    if (b1->decimal > b2->decimal) {
        skip2 = b1->decimal - b2->decimal;
    } else {
        skip1 = b2->decimal - b1->decimal;
    }

    int i = 0, j = 0, k = 0, a = 0, b = 0;
    // c is for carry

    while ((i - skip1) < b1->size || (j - skip2) < b2->size) {
        if (i - skip1 < 0 || i - skip1 >= b1->size) {
            a = 0;
            b = b2->revdigits[j - skip2] - '0';
        } else if (j - skip2 < 0 || j - skip2 >= b2->size) {
            a = b1->revdigits[i - skip1] - '0';
            b = 0;
        } else {
            a = b1->revdigits[i - skip1] - '0';
            b = b2->revdigits[j - skip2] - '0';
        }



        ans->revdigits[k] = ((a + b + c) % 10) + '0';
        c = (a + b + c) / 10;

        i += 1;
        j += 1;
        k += 1;
    }
    ans->revdigits[k] = c + '0';
}

void generateZero (bignum *zero) {
    zero->decimal = 0;
    zero->size = 0;
    zero->sign = PLUS;
}

void complementOfNonZero(bignum *bi) {
    for (int i=0; i < bi->size; i++) {
        bi->revdigits[i] = '9' - bi->revdigits[i] + '0';
    }
    bignum zero;
    generateZero(&zero);
    uadd(bi, &zero, bi, 1, 0);
    // only overflow problem can occur when bi digits are all zero, that will not be the case in subtraction
}

void usub(bignum *b1, bignum *b2, bignum *ans) {
    ans->decimal = max(b1->decimal, b2->decimal);
    int length = max(b1->size - b1->decimal,b2->size - b2->decimal) + ans->decimal ;
    ans->size = length;
    ans->sign = PLUS;
    ans->revdigits = malloc(length * sizeof(char));

    int skip1 = 0;
    int skip2 = 0;

    if (b1->decimal > b2->decimal) {
        skip2 = b1->decimal - b2->decimal;
    } else {
        skip1 = b2->decimal - b1->decimal;
    }

    int i = 0, j = 0, k = 0, a = 0, b = 0, c = 0, temp;
    // c is for carry

    while ((i - skip1) < b1->size || (j - skip2) < b2->size) {
        if (i - skip1 < 0 || i - skip1 >= b1->size) {
            a = 0;
            b = b2->revdigits[j - skip2] - '0';
        } else if (j - skip2 < 0 || j - skip2 >= b2->size) {
            a = b1->revdigits[i - skip1] - '0';
            b = 0;
        } else {
            a = b1->revdigits[i - skip1] - '0';
            b = b2->revdigits[j - skip2] - '0';
        }

        ans->revdigits[k] = 7;

        temp = a - b - c;

        if (temp < 0) {
            temp = 10 + a - b - c;
            c = 1;
        }
        else c = 0;

        ans->revdigits[k] = (temp) + '0';

        i += 1;
        j += 1;
        k += 1;
    }
    if (c) {
        ans->sign = MINUS;
        complementOfNonZero(ans);
    }
}

void usinglemult (bignum *b1, char b, bignum *ans, int shift) {
    ans->size =  b1->size + shift + 1;
    ans->revdigits = malloc(ans->size * sizeof(char));
    ans->sign = b1->sign;
    ans->decimal = b1->decimal;

    int c;
    int temp;
    int i=0;
    for (i=0; i<shift; i++) {
        ans->revdigits[i] = '0';
    }
    for(i=0; i<b1->size; i++) {
        temp = (b1->revdigits[i] - '0') * (b - '0');
        ans->revdigits[i+shift] = (temp % 10) + '0';
        c =  (temp / 10);
    }
    ans->revdigits[i+shift] = c + '0';
}

void umultiply (bignum *b1, bignum *b2, bignum *ans) {
    int length = b1->size + b2->size;
    ans->size =  length;
    ans->decimal = b1->decimal; // only temporary for proper uadd functionality
    ans->revdigits = malloc(length * sizeof(char));

    bignum temp;
    int i=0;
    for (i=0;i<ans->size;i++) {
        ans->revdigits[i] = '0';
    }
    for (i=0; i<b2->size; i++) {
        usinglemult(b1, b2->revdigits[i], &temp, i);
        uadd(ans, &temp, ans, 0, 0);
        free(temp.revdigits);
    }

    if (b1->sign == PLUS && b2->sign == PLUS || b1->sign == MINUS && b2->sign == MINUS) ans->sign = PLUS;
    else ans->sign = MINUS;
    ans->decimal = b1->decimal + b2->decimal;
}

int max(int n1, int n2) {
    if (n1 > n2) return n1;
    else return n2;
}

void initialize_bignum(bignum *bi, char *str) {
    int length = strlen(str);
    int i = 0;
    int j = 0;
    int containDecimal = 0;
    bi->decimal = 0;
    if (length > 0) {

        for (i = 0; i < length; i++) {
            if (str[i] == '.') containDecimal = 1;
        }

        if (containDecimal) {
            length--;
        }

        if (str[0] == '-') { // Negative Number
            bi->sign = MINUS;
            length--;
            bi->size = length;
            bi->revdigits = malloc(length * sizeof(char));
        } else { // Positive Number
            bi->sign = PLUS;
            bi->size = length;
            bi->revdigits = malloc(length * sizeof(char));
        }

        int isMinus = (bi->sign == MINUS) ? 1 : 0;

        if (bi->revdigits != NULL) {
            int strlength = strlen(str);
            for (i = isMinus, j = bi->size - 1; i < strlength; i++) {
                if (str[i] == '.') {
                    bi->decimal = 1 + j;
                    i += 1;
                }
                if (str[i] >= '0' && str[i] <= '9') {
                    bi->revdigits[j] = str[i];
                    j--;
                } else {
                    printf("error: string format wrong: -(first alphabet) or . (1 repeatition) or 0 to 9\n");
                }
            }
        } else {
            printf("error: insufficient storage");
        }
    } else {
        printf("error: bignum can't be initialized from empty string");
    }
}

void print_bignum(bignum *bi) {
    if (bi->sign == MINUS) {
        printf("-");
    }
    for (int i = bi->size-1; i >=0; i--) {
        printf("%c", bi->revdigits[i]);
        if (i == bi->decimal) printf(".");
    }
    printf("\n");
}

int main() {
    printf("Hello, World!\n");
    bignum b1;
    bignum b2;
    bignum ans;
    initialize_bignum(&b1, "932.32");
    initialize_bignum(&b2, "323333.");

    print_bignum(&b1);
    print_bignum(&b2);
    umultiply(&b1, &b2, &ans);
    print_bignum(&ans);
    return 0;
}

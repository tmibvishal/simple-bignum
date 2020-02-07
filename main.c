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

 take divide by 0 case
*/

struct bignum_struct {
    char *revdigits; // digits are stored in reverse order
    int sign;
    int size;
    int decimal;
} bignum_default = {NULL, PLUS, 0, 0};

typedef struct bignum_struct bignum;

int max(int n1, int n2) {
    if (n1 > n2) return n1;
    else return n2;
}

char* mallocWithError(int length) {
    char* ptr = malloc(length+1);
    if (ptr != NULL) {
        ptr[length] = '\0';
        return ptr;
    } else {
        printf("error: insufficient storage");
        exit(1);
    }
}

void freeMallocSpace (bignum* bi) {
    if (bi->revdigits != NULL) {
        free(bi->revdigits);
        bi->revdigits = NULL;
    }
}

void fixLeadingZero(bignum *bi) {
    int i = bi->size - 1;
    while (i > bi->decimal) {
        if (bi->revdigits[i]=='0') {
            bi->revdigits[i] = '\0';
            bi->size -= 1;
        } else {
            break;
        }
        i -= 1;
    }
}

int isZero(bignum *bi) {
    for (int i=0; i<bi->size; i++) {
        if (bi->revdigits[i] != '0') return 0;
    }
    return 1;
}

int isOne(bignum *bi) {
    fixLeadingZero(bi);
    if (bi->size - bi->decimal != 1 || bi->revdigits[bi->size-1] != '1') return 0;
    for (int i=0; i<bi->decimal;i++) {
        if (bi->revdigits[i] != '0') return 0;
    }
    return 1;
}

void generateZero(bignum *zero) {
    zero->decimal = 0;
    zero->size = 0;
    zero->sign = PLUS;
}


void generateOne(bignum *one) {
    one->decimal = 0;
    one->size = 1;
    one->sign = PLUS;
    one->revdigits = mallocWithError(1 * sizeof(char));
    one->revdigits[0] = '1';
}


void initialize_bignum(bignum *bi, char *str) {
    freeMallocSpace(bi);
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
            bi->revdigits = mallocWithError(length * sizeof(char));
        } else { // Positive Number
            bi->sign = PLUS;
            bi->size = length;
            bi->revdigits = mallocWithError(length * sizeof(char));
        }

        int isMinus = (bi->sign == MINUS) ? 1 : 0;

        if (bi->revdigits != NULL) {
            int strlength = strlen(str);
            for (i = isMinus, j = bi->size - 1; i < strlength; i++) {
                if (str[i] == '.') {
                    bi->decimal = 1 + j;
                    i += 1;
                }
                if (i == strlength){
                    break;
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

void uadd(bignum *b1, bignum *b2, bignum *ans, int c, int initialize);
void copyBignum(bignum *copyFrom, bignum *copyTo, int noOfMFDtoCopy, int doRounding) {
    // freeing space for revdigits of old copyTo
    freeMallocSpace(copyTo);
    if (noOfMFDtoCopy == -1) {
        copyTo->size = copyFrom->size;
        copyTo->decimal = copyFrom->decimal;
        copyTo->sign = copyFrom->sign;
        copyTo->revdigits = mallocWithError(copyTo->size * sizeof(char));
        for (int i = 0; i < copyTo->size; i++) {
            copyTo->revdigits[i] = copyFrom->revdigits[i];
        }
    } else {
        copyTo->size = noOfMFDtoCopy;
        copyTo->decimal = noOfMFDtoCopy - (copyFrom->size - copyFrom->decimal);
        if (copyTo->decimal < 0) {
            copyTo->decimal = 0;
        }
        copyTo->sign = copyFrom->sign;
        copyTo->revdigits = mallocWithError(copyTo->size * sizeof(char));
        int k = copyFrom->size - noOfMFDtoCopy;

        for (int i = 0; i < noOfMFDtoCopy; i++) {
            if (i + k < 0) {
                copyTo->revdigits[i] = '0';
            } else {
                copyTo->revdigits[i] = copyFrom->revdigits[i + k];
            }
        }
        
        
        if (k>=1 && doRounding) {
            if (copyFrom->revdigits[k - 1] > '5') {
//                char s = "0";
//                int i=0;
//                for (;i <copyTo->decimal-1; i++) {
//                    s[i] = '0';
//                }
//                s[i] = '1';

                bignum zero = bignum_default;
                generateZero(&zero);

                uadd(copyTo, &zero, copyTo, 1, 0);
            }
        }
    }
}

void uadd(bignum *b1, bignum *b2, bignum *ans, int c, int initialize) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    copyBignum(b1, &b1Duplicate, -1, 0);

    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    copyBignum(b2, &b2Duplicate, -1, 0);

    if (initialize) {
        freeMallocSpace(ans);
        ans->decimal = max(b1Duplicate.decimal, b2Duplicate.decimal);
        int length =
                max(b1Duplicate.size - b1Duplicate.decimal, b2Duplicate.size - b2Duplicate.decimal) + ans->decimal + 1;
        ans->size = length;
        ans->sign = PLUS;
        ans->revdigits = mallocWithError(length * sizeof(char));
    }

    int skip1 = 0;
    int skip2 = 0;

    if (b1Duplicate.decimal > b2Duplicate.decimal) {
        skip2 = b1Duplicate.decimal - b2Duplicate.decimal;
    } else {
        skip1 = b2Duplicate.decimal - b1Duplicate.decimal;
    }

    int i = 0, j = 0, k = 0, a = 0, b = 0;
    // c is for carry

    while ((i - skip1) < b1Duplicate.size || (j - skip2) < b2Duplicate.size) {
        if (i - skip1 < 0 || i - skip1 >= b1Duplicate.size) {
            a = 0;
            b = b2Duplicate.revdigits[j - skip2] - '0';
        } else if (j - skip2 < 0 || j - skip2 >= b2Duplicate.size) {
            a = b1Duplicate.revdigits[i - skip1] - '0';
            b = 0;
        } else {
            a = b1Duplicate.revdigits[i - skip1] - '0';
            b = b2Duplicate.revdigits[j - skip2] - '0';
        }


        ans->revdigits[k] = ((a + b + c) % 10) + '0';
        c = (a + b + c) / 10;

        i += 1;
        j += 1;
        k += 1;
    }
    ans->revdigits[k] = c + '0';
    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
}



void complementOfNonZero(bignum *bi) {
    for (int i = 0; i < bi->size; i++) {
        bi->revdigits[i] = '9' - bi->revdigits[i] + '0';
    }
    bignum zero = bignum_default;
    generateZero(&zero);
    uadd(bi, &zero, bi, 1, 0);
    // only overflow problem can occur when bi digits are all zero, that will not be the case in subtraction
}

void appendIgnoringDecimal(bignum *b1, bignum *b2, bignum *ans) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    copyBignum(b1, &b1Duplicate, -1, 0);

    bignum b2Duplicate = bignum_default; // in case b1 and ans are same
    copyBignum(b2, &b2Duplicate, -1, 0);

    freeMallocSpace(ans);

    int length = b1Duplicate.size + b2Duplicate.size;
    ans->decimal = 0;
    ans->sign = PLUS;
    ans->size = length;
    ans->revdigits = mallocWithError(length * sizeof(char));
    int i = 0;
    for (;i<b2Duplicate.size;i++) {
        ans->revdigits[i] = b2Duplicate.revdigits[i];
    }
    int j = 0;
    for (;j<b1Duplicate.size;j++) {
        ans->revdigits[i+j] = b1Duplicate.revdigits[j];
    }
}

void usub(bignum *b1, bignum *b2, bignum *ans) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    copyBignum(b1, &b1Duplicate, -1, 0);

    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    copyBignum(b2, &b2Duplicate, -1, 0);


    ans->decimal = max(b1Duplicate.decimal, b2Duplicate.decimal);
    int length = max(b1Duplicate.size - b1Duplicate.decimal, b2Duplicate.size - b2Duplicate.decimal) + ans->decimal;
    ans->size = length;
    ans->sign = PLUS;
    ans->revdigits = mallocWithError(length * sizeof(char));

    int skip1 = 0;
    int skip2 = 0;

    if (b1Duplicate.decimal > b2Duplicate.decimal) {
        skip2 = b1Duplicate.decimal - b2Duplicate.decimal;
    } else {
        skip1 = b2Duplicate.decimal - b1Duplicate.decimal;
    }

    int i = 0, j = 0, k = 0, a = 0, b = 0, c = 0, temp;
    // c is for carry

    while ((i - skip1) < b1Duplicate.size || (j - skip2) < b2Duplicate.size) {
        if (i - skip1 < 0 || i - skip1 >= b1Duplicate.size) {
            a = 0;
            b = b2Duplicate.revdigits[j - skip2] - '0';
        } else if (j - skip2 < 0 || j - skip2 >= b2Duplicate.size) {
            a = b1Duplicate.revdigits[i - skip1] - '0';
            b = 0;
        } else {
            a = b1Duplicate.revdigits[i - skip1] - '0';
            b = b2Duplicate.revdigits[j - skip2] - '0';
        }

        ans->revdigits[k] = 7;

        temp = a - b - c;

        if (temp < 0) {
            temp = 10 + a - b - c;
            c = 1;
        } else c = 0;

        ans->revdigits[k] = (temp) + '0';

        i += 1;
        j += 1;
        k += 1;
    }
    if (c) {
        ans->sign = MINUS;
        complementOfNonZero(ans);
    }

    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
}

void sadd(bignum *b1, bignum *b2, bignum *ans) { // signed add
    if (b1->sign == b2->sign) {
        uadd(b1, b2, ans, 0, 0);
        ans->sign = b1->sign;
    } else {
        if (b2->sign == MINUS) {
            usub(b1, b2, ans);
        } else {
            usub(b2, b1, ans);
        }
    }
    fixLeadingZero(ans);
}

void ssub(bignum *b1, bignum *b2, bignum *ans) { // signed sub
    if (b1->sign == b2->sign) {
        usub(b1, b2, ans);
        if (b1->sign == MINUS) {
            ans->sign = ans->sign == PLUS ? MINUS : PLUS;
        }
    } else {
        if (b2->sign == MINUS) {
            uadd(b1, b2, ans, 0, 0);
        } else {
            uadd(b2, b1, ans, 0, 0);
            ans->sign = PLUS;
        }
    }
    fixLeadingZero(ans);
}

void usinglemult(bignum *b1, char b, bignum *ans, int shift) {

    // freeing the memory from the old revdigits of ans if any
    freeMallocSpace(ans);

    ans->size = b1->size + shift + 1;
    ans->revdigits = mallocWithError(ans->size * sizeof(char));
    ans->sign = b1->sign;
    ans->decimal = b1->decimal;

    int c = 0;
    int temp;
    int i = 0;
    for (i = 0; i < shift; i++) {
        ans->revdigits[i] = '0';
    }
    for (i = 0; i < b1->size; i++) {
        temp = (b1->revdigits[i] - '0') * (b - '0') + c;
        ans->revdigits[i + shift] = (temp % 10) + '0';
        c = (temp / 10);
    }
    ans->revdigits[i + shift] = c + '0';
}

void smultiply(bignum *b1, bignum *b2, bignum *ans) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    copyBignum(b1, &b1Duplicate, -1, 0);

    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    copyBignum(b2, &b2Duplicate, -1, 0);

    freeMallocSpace(ans);

    int length = b1Duplicate.size + b2Duplicate.size;
    ans->size = length;
    ans->decimal = b1Duplicate.decimal; // only temporary for proper uadd functionality
    ans->revdigits = mallocWithError(length * sizeof(char));

    bignum temp = bignum_default;
    int i = 0;
    for (i = 0; i < ans->size; i++) {
        ans->revdigits[i] = '0';
    }
    for (i = 0; i < b2Duplicate.size; i++) {
        usinglemult(&b1Duplicate, b2Duplicate.revdigits[i], &temp, i);
        uadd(ans, &temp, ans, 0, 0);
    }

    if (b1Duplicate.sign == PLUS && b2Duplicate.sign == PLUS ||
        b1Duplicate.sign == MINUS && b2Duplicate.sign == MINUS)
        ans->sign = PLUS;
    else ans->sign = MINUS;
    ans->decimal = b1Duplicate.decimal + b2Duplicate.decimal;

    fixLeadingZero(ans);

    freeMallocSpace(&temp);
    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
}

void multiplyBy10Pow(bignum *from, bignum *to, int power) {

    bignum fromDuplicate = bignum_default; // in case from and to are same
    copyBignum(from, &fromDuplicate, -1, 0);

    char *s = mallocWithError((power + 2) * sizeof(char));
    s[0] = '0' + 1;
    int i = 1;
    for (; i < power+1; i++) {
        s[i] = '0' + 0;
    }
    s[i] = '\0';
    bignum temp = bignum_default;
    initialize_bignum(&temp, s);
    smultiply(&fromDuplicate, &temp, &fromDuplicate);
    copyBignum(&fromDuplicate, to, -1, 0);

    freeMallocSpace(&fromDuplicate);
}

void sdivide(bignum *b1, bignum *b2, bignum *ans, int precision, int roundOffLastDigit) {
    // b1 is dividend and b2 is divisor
    bignum dividend = bignum_default;
    copyBignum(b1, &dividend, -1, 0);
    bignum divisor = bignum_default;
    copyBignum(b2, &divisor, -1, 0);


    int ansSign;
    if (dividend.sign == divisor.sign)
        ansSign = PLUS;
    else ansSign = MINUS;

    bignum tempans = bignum_default;

    // making dividend->decimal and divisor->decimal 0
    int extradecimal = b1->decimal - b2->decimal;
    if (extradecimal > 0) {
        multiplyBy10Pow(&divisor, &tempans, extradecimal);
        copyBignum(&tempans, &divisor, -1, 0);
        dividend.sign = PLUS;
        divisor.sign = PLUS;
        dividend.decimal = 0;
        divisor.decimal = 0;
    } else {
        extradecimal = -1 * extradecimal;
        multiplyBy10Pow(&dividend, &tempans, extradecimal);
        copyBignum(&tempans, &dividend, -1, 0);
        dividend.sign = PLUS;
        divisor.sign = PLUS;
        dividend.decimal = 0;
        divisor.decimal = 0;
    }
    int tempPrecision = roundOffLastDigit ? precision+1 : precision;
    multiplyBy10Pow(&dividend, &tempans, tempPrecision);
    copyBignum(&tempans, &dividend, -1, 0);



    bignum temp = bignum_default;
    bignum subtractionResult = bignum_default;

    int dividendSize = dividend.size; // because it changes while division
    char* ansStr = mallocWithError((dividendSize+1) * sizeof(char));
    int totalCounter = 1;
    for (totalCounter = 1; totalCounter <= dividendSize; totalCounter++) {
        int i = 0;
        for (; i <= 9; i++) {
            char str[3];
            if (i == 9) {
                str[0] = '0' + 1; // don't remove 0 from here
                str[1] = '0' + 0; // don't remove 0 from here
                str[2] = '\0';
            } else {
                str[0] = '0' + i + 1; // don't remove 0 from here
                str[1] = '\0';
            }

            initialize_bignum(&temp, str);
            smultiply(&divisor, &temp, &temp);
            multiplyBy10Pow(&temp, &temp, dividendSize - totalCounter);
            usub(&dividend, &temp, &subtractionResult);

            if (subtractionResult.sign == MINUS) {
                str[0] = '0' + i; // don't remove 0 from here
                str[1] = '\0';
                initialize_bignum(&temp, str);;
                smultiply(&divisor, &temp, &temp);
                multiplyBy10Pow(&temp, &temp, dividendSize - totalCounter);
                usub(&dividend, &temp, &dividend);
//                multiplyBy10Pow(&divisor, &temp, dividendSize - totalCounter);
//                uadd(&dividend, &temp, &dividend, 0, 0);
                break;
            }
        }
        ansStr[totalCounter - 1] = '0' + i;
    }
    ansStr[totalCounter - 1] = '\0';

    if (roundOffLastDigit == 1) {
        initialize_bignum(&temp, ansStr);
        temp.decimal = precision + 1;
        copyBignum(&temp, ans, temp.size-1, 1); // rounding off one extra precision made
    } else {
        initialize_bignum(ans, ansStr);
    }
    ans->decimal = precision;
    ans->sign = ansSign;

    fixLeadingZero(ans);

    freeMallocSpace(&tempans);
    freeMallocSpace(&temp);
    freeMallocSpace(&divisor);
    freeMallocSpace(&subtractionResult);
    freeMallocSpace(&dividend);
}

void usquareRoot(bignum *b1, bignum *ans, int precision, int roundOffLastDigit) {
    // making a copy in dividend and making decimal places 0 and making a 0 initial divisor
    bignum divisor = bignum_default;
    generateZero(&divisor);
    bignum dividend = bignum_default;
    if (b1->decimal % 2 != 0) {
        copyBignum(b1, &dividend, b1->size+1, 0);
    } else {
        copyBignum(b1, &dividend, -1, 0);
    }

    int tempPrecision = roundOffLastDigit ? (2*precision - dividend.decimal + 2) : (2*precision - dividend.decimal);
    int ansDecimal = precision;
    int ansSign = dividend.sign;
    dividend.decimal = 0;
    bignum tempans = bignum_default;
    multiplyBy10Pow(&dividend, &tempans, tempPrecision);
    copyBignum(&tempans, &dividend, -1, 0);
    fixLeadingZero(&dividend);



    bignum temp = bignum_default;
    bignum tempmult = bignum_default;
    bignum subtractionResult = bignum_default;

    int dividendSize = dividend.size; // because it changes while division
    char* ansStr = mallocWithError((dividendSize+1) * sizeof(char));

    int totalCounter;
    int strCounter = 0;
    if (dividend.size % 2 ==0 ) {
        totalCounter = 2;
    } else {
        totalCounter = 1;
    }
    for (; totalCounter <= dividendSize; totalCounter+=2) {
        int i = 0;
        for (; i <= 9; i++) {
            char str[3];
            if (i == 9) {
                str[0] = '0' + 1; // don't remove 0 from here
                str[1] = '0' + 0; // don't remove 0 from here
                str[2] = '\0';
            } else {
                str[0] = '0' + i + 1; // don't remove 0 from here
                str[1] = '\0';
            }

            initialize_bignum(&temp, str);
            appendIgnoringDecimal(&divisor, &temp, &tempmult);
            smultiply(&tempmult, &temp, &tempmult);
            multiplyBy10Pow(&tempmult, &tempmult, dividendSize - totalCounter);
            usub(&dividend, &tempmult, &subtractionResult);

            if (subtractionResult.sign == MINUS) {
                str[0] = '0' + i; // don't remove 0 from here
                str[1] = '\0';
                initialize_bignum(&temp, str);
                appendIgnoringDecimal(&divisor, &temp, &divisor);
                smultiply(&divisor, &temp, &tempmult);
                uadd(&divisor, &temp, &divisor, 0, 1);
                multiplyBy10Pow(&tempmult, &tempmult, dividendSize - totalCounter);
                usub(&dividend, &tempmult, &dividend);
                break;
            }
        }
        ansStr[strCounter] = '0' + i;
        strCounter += 1;
    }
    ansStr[strCounter] = '\0';

    if (roundOffLastDigit == 1) {
        initialize_bignum(&temp, ansStr);
        temp.decimal = precision + 1;
        copyBignum(&temp, ans, temp.size-1, 1); // rounding off one extra precision made
    } else {
        initialize_bignum(ans, ansStr);
    }
    ans->decimal = ansDecimal;
    ans->sign = ansSign;

    fixLeadingZero(ans);

    freeMallocSpace(&tempans);
    freeMallocSpace(&temp);
    freeMallocSpace(&tempmult);
    freeMallocSpace(&divisor);
    freeMallocSpace(&subtractionResult);
    freeMallocSpace(&dividend);
}

void powerHelper(bignum *b1, bignum *b2, bignum *ans, int precision) {
    if (b2->decimal != 0) {
        printf("error: currently power only works with integers in power");
        exit(1);
    }
    if (isZero(b2)) {
        generateOne(ans);
        return;
    }
    if (isOne(b2)) {
        copyBignum(b1, ans, -1, 0);
        return;
    }

    if (b2->size > 0) {
        int lsb = b2->revdigits[0] - '0';
        if(lsb % 2 == 0){
            char *s = mallocWithError((2) * sizeof(char));
            s[0] = '2';
            s[1] = '\0';
            bignum temp = bignum_default;
            initialize_bignum(&temp, s);
            sdivide(b2, &temp, b2, 0, 0);
            powerHelper(b1, b2, ans, precision);
            smultiply(ans, ans, ans);
        } else {
            char *s = mallocWithError((2) * sizeof(char));
            s[0] = '2';
            s[1] = '\0';
            bignum temp = bignum_default;
            initialize_bignum(&temp, s);
            sdivide(b2, &temp, b2, 0, 0);
            powerHelper(b1, b2, ans, precision);
            smultiply(ans, ans, ans);
            smultiply(ans, b1, ans);
        }
    }
}

void absolute(bignum *bi) {
    bi->sign = PLUS;
}

void spower(bignum *b1, bignum *b2, bignum *ans, int precision, int roundingOff) {

    bignum b1Duplicate = bignum_default; // in case from and to are same
    copyBignum(b1, &b1Duplicate, -1, 0);

    bignum b2Duplicate = bignum_default; // in case from and to are same
    copyBignum(b2, &b2Duplicate, -1, 0);

    freeMallocSpace(ans);

    int anssign = PLUS;
    if (b2Duplicate.size > 0) {
        int lsb = b2Duplicate.revdigits[0] - '0';
        if (lsb % 2 == 0) {
            anssign = PLUS;
        } else {
            anssign = MINUS;
        }
    }
    if (b2Duplicate.size == MINUS) {
        bignum temp = bignum_default;
        generateOne(&temp);
        sdivide(&temp, &b1Duplicate, &b1Duplicate, 2*precision, 0);
    }
    powerHelper(&b1Duplicate, &b2Duplicate, ans, precision);
    if (ans->decimal > (precision)) {
        bignum temp2 = bignum_default;
        copyBignum(ans, &temp2, ans->size - ans->decimal + precision, roundingOff);
        copyBignum(&temp2, ans, -1, 0);
        freeMallocSpace(&temp2);
    }
    ans->sign = anssign;
    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
}

void print_bignum(bignum *bi) {
    if (bi->sign == MINUS) {
        printf("-");
    }
    for (int i = bi->size - 1; i >= 0; i--) {
        printf("%c", bi->revdigits[i]);
        if (i == bi->decimal) printf(".");
    }
    printf("\n");
}

int main() {
    printf("Hello, World!\n");
    bignum b1 = bignum_default;
    bignum b2 = bignum_default;
    bignum ans = bignum_default;
    initialize_bignum(&b1, "2.");
    initialize_bignum(&b2, "-2.");

    absolute(&b2);

    print_bignum(&b1);
    print_bignum(&b2);
    // sdivide(&b1, &b2, &ans, 20, 0);
    // usquareRoot(&b1, &ans, 20, 0);
    // smultiply(&b1, &b2, &ans);
    spower(&b1, &b2, &ans, 20, 0);
    print_bignum(&ans);
    return 0;
}

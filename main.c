#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PLUS 0
#define MINUS 1

#ifndef MAXSIZE
#define MAXSIZE 50001
#endif

/*

Notes:
 free(malloc) memories
 use error: no space on mallow ptr null
 not print starting 0 or trailing 0

 take divide by 0 case

 change abs parameters to input ans and
 change makezero to makeempty

Errors:
error 1:
    error: insufficient storage

error 2:
    error: string format wrong: -(first alphabet) or . (1 repeatition) or 0 to 9

error 3:
    error: bignum can't be initialized from empty string

error 4:
    error: division by 0 is not allowed

error 5:
    error: currently power only works with integers in power

error 6:
    error: only integers factorial can be calculated
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
    char *ptr = (char *) malloc(length + 1);
    if (ptr != NULL) {
        ptr[length] = '\0';

    }
    return ptr;
}

void freeMallocSpace(bignum *bi) {
    if (bi->revdigits != NULL) {
        free(bi->revdigits);
        bi->revdigits = NULL;
    }
}

void fixLeadingZero(bignum *bi) {
    int i = bi->size - 1;
    while (i > bi->decimal) {
        if (bi->revdigits[i] == '0') {
            bi->revdigits[i] = '\0';
            bi->size -= 1;
        } else {
            break;
        }
        i -= 1;
    }
}

int isZero(bignum *bi) {
    for (int i = 0; i < bi->size; i++) {
        if (bi->revdigits[i] != '0') return 0;
    }
    return 1;
}

int isOne(bignum *bi) {
    fixLeadingZero(bi);
    if (bi->size - bi->decimal != 1 || bi->revdigits[bi->size - 1] != '1') return 0;
    for (int i = 0; i < bi->decimal; i++) {
        if (bi->revdigits[i] != '0') return 0;
    }
    return 1;
}

void generateEmpty(bignum *biempty) {
    biempty->decimal = 0;
    biempty->size = 0;
    biempty->sign = PLUS;
}

int generateZero(bignum *one) {
    one->decimal = 0;
    one->size = 1;
    one->sign = PLUS;
    one->revdigits = mallocWithError(1 * sizeof(char));
    one->revdigits[0] = '0';
    if (one->revdigits == NULL) {
        return 1;
    }
}

int generateOne(bignum *one) {
    one->decimal = 0;
    one->size = 1;
    one->sign = PLUS;
    one->revdigits = mallocWithError(1 * sizeof(char));
    if (one->revdigits == NULL) {
        return 1;
    }
    one->revdigits[0] = '1';
    return 0;
}


int initialize_bignum(bignum *bi, char *str) {
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
                if (i == strlength) {
                    break;
                }
                if (str[i] >= '0' && str[i] <= '9') {
                    bi->revdigits[j] = str[i];
                    j--;
                } else {
                    return 2;
                }
            }
        } else {
            return 1;
        }
        return 0;
    } else {
        return 3;
    }
}

int uadd(bignum *b1, bignum *b2, bignum *ans, int c, int initialize);

int copyBignum(bignum *copyFrom, bignum *copyTo, int noOfMFDtoCopy, int doRounding) {
    // freeing space for revdigits of old copyTo
    freeMallocSpace(copyTo);
    int e = 0;
    if (noOfMFDtoCopy == -1) {
        copyTo->size = copyFrom->size;
        copyTo->decimal = copyFrom->decimal;
        copyTo->sign = copyFrom->sign;
        copyTo->revdigits = mallocWithError(copyTo->size * sizeof(char));
        if (copyTo->revdigits == NULL) {
            return 1;
        }
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
        if (copyTo->revdigits == NULL) {
            return 1;
        }
        int k = copyFrom->size - noOfMFDtoCopy;
        for (int i = 0; i < noOfMFDtoCopy; i++) {
            if (i + k < 0) {
                copyTo->revdigits[i] = '0';
            } else {
                copyTo->revdigits[i] = copyFrom->revdigits[i + k];
            }
        }
        if (k >= 1 && doRounding) {
            if (copyFrom->revdigits[k - 1] > '5') {
                bignum zero = bignum_default;
                generateEmpty(&zero);
                e = uadd(copyTo, &zero, copyTo, 1, 0);
                if (e) return e;
            }
        }
    }
    return 0;
}

int fixTrailingZero(bignum *bi) {
    int i = 0;
    while (i < bi->decimal) {
        if (bi->revdigits[i] == '0') i += 1;
        else break;
    }
    // i is no of trailing 0
    bignum temp = bignum_default;
    int e = copyBignum(bi, &temp, -1, 0);
    if (e) return e;
    e = copyBignum(&temp, bi, temp.size - i, 0);
    if (e) return e;
    return 0;
}

int uadd(bignum *b1, bignum *b2, bignum *ans, int c, int initialize) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    int e = copyBignum(b1, &b1Duplicate, -1, 0);
    if (e) return e;

    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    e = copyBignum(b2, &b2Duplicate, -1, 0);
    if (e) return e;

    if (initialize) {
        freeMallocSpace(ans);
        ans->decimal = max(b1Duplicate.decimal, b2Duplicate.decimal);
        int length =
                max(b1Duplicate.size - b1Duplicate.decimal, b2Duplicate.size - b2Duplicate.decimal) + ans->decimal + 1;
        ans->size = length;
        ans->sign = PLUS;
        ans->revdigits = mallocWithError(length * sizeof(char));
        if (ans->revdigits == NULL) return 1;
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
    return 0;
}


int twoComplementOfNonZero(bignum *bi) {
    for (int i = 0; i < bi->size; i++) {
        bi->revdigits[i] = '9' - bi->revdigits[i] + '0';
    }
    bignum zero = bignum_default;
    generateEmpty(&zero);
    int e = uadd(bi, &zero, bi, 1, 0);
    if (e) return e;
    return 0;
    // only overflow problem can occur when bi digits are all zero, that will not be the case in subtraction
}

int appendIgnoringDecimal(bignum *b1, bignum *b2, bignum *ans) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    int e = copyBignum(b1, &b1Duplicate, -1, 0);
    if (e) return e;
    bignum b2Duplicate = bignum_default; // in case b1 and ans are same
    e = copyBignum(b2, &b2Duplicate, -1, 0);
    if (e) return e;
    freeMallocSpace(ans);

    int length = b1Duplicate.size + b2Duplicate.size;
    ans->decimal = 0;
    ans->sign = PLUS;
    ans->size = length;
    ans->revdigits = mallocWithError(length * sizeof(char));
    if (ans->revdigits == NULL) return 1;
    int i = 0;
    for (; i < b2Duplicate.size; i++) {
        ans->revdigits[i] = b2Duplicate.revdigits[i];
    }
    int j = 0;
    for (; j < b1Duplicate.size; j++) {
        ans->revdigits[i + j] = b1Duplicate.revdigits[j];
    }
    return 0;
}

int usub(bignum *b1, bignum *b2, bignum *ans) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    int e = copyBignum(b1, &b1Duplicate, -1, 0);
    if (e) return e;
    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    e = copyBignum(b2, &b2Duplicate, -1, 0);
    if (e) return e;

    ans->decimal = max(b1Duplicate.decimal, b2Duplicate.decimal);
    int length = max(b1Duplicate.size - b1Duplicate.decimal, b2Duplicate.size - b2Duplicate.decimal) + ans->decimal;
    ans->size = length;
    ans->sign = PLUS;
    ans->revdigits = mallocWithError(length * sizeof(char));
    if (ans -> revdigits == NULL) return 1;

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
        e = twoComplementOfNonZero(ans);
        if (e) return e;
    }
    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
    return 0;
}

int sadd(bignum *b1, bignum *b2, bignum *ans) { // signed add
    int e = 0;
    if (b1->sign == b2->sign) {
        e = uadd(b1, b2, ans, 0, 1);
        ans->sign = b1->sign;
    } else {
        if (b2->sign == MINUS) {
            e = usub(b1, b2, ans);
        } else {
            e = usub(b2, b1, ans);
        }
    }
    if (e) return e;
    fixLeadingZero(ans);
    return 0;
}

int ssub(bignum *b1, bignum *b2, bignum *ans) { // signed sub
    int e = 0;
    if (b1->sign == b2->sign) {
        e = usub(b1, b2, ans);
        if (b1->sign == MINUS) {
            ans->sign = ans->sign == PLUS ? MINUS : PLUS;
        }
    } else {
        if (b2->sign == MINUS) {
            e = uadd(b1, b2, ans, 0, 1);
        } else {
            e = uadd(b2, b1, ans, 0, 1);
            ans->sign = PLUS;
        }
    }
    if (e) return e;
    fixLeadingZero(ans);
    return 0;
}

int usinglemult(bignum *b1, char b, bignum *ans, int shift) {
    // freeing the memory from the old revdigits of ans if any
    freeMallocSpace(ans);
    ans->size = b1->size + shift + 1;
    ans->revdigits = mallocWithError(ans->size * sizeof(char));
    if (ans->revdigits == NULL) return 1;
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
    return 0;
}

int smultiply(bignum *b1, bignum *b2, bignum *ans) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    int e = copyBignum(b1, &b1Duplicate, -1, 0);
    if (e) return e;
    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    e = copyBignum(b2, &b2Duplicate, -1, 0);
    if (e) return e;
    freeMallocSpace(ans);

    int length = b1Duplicate.size + b2Duplicate.size;
    ans->size = length;
    ans->decimal = b1Duplicate.decimal; // only temporary for proper uadd functionality
    ans->revdigits = mallocWithError(length * sizeof(char));
    if (ans->revdigits == NULL) return 1;

    bignum temp = bignum_default;
    int i = 0;
    for (i = 0; i < ans->size; i++) {
        ans->revdigits[i] = '0';
    }
    for (i = 0; i < b2Duplicate.size; i++) {
        e = usinglemult(&b1Duplicate, b2Duplicate.revdigits[i], &temp, i);
        if (e) return e;
        e = uadd(ans, &temp, ans, 0, 0);
        if (e) return e;
    }

    if (b1Duplicate.sign == b2Duplicate.sign)
        ans->sign = PLUS;
    else ans->sign = MINUS;
    ans->decimal = b1Duplicate.decimal + b2Duplicate.decimal;

    fixLeadingZero(ans);
    freeMallocSpace(&temp);
    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
    return 0;
}

int multiplyBy10Pow(bignum *from, bignum *to, int power) {

    bignum fromDuplicate = bignum_default; // in case from and to are same
    int e = copyBignum(from, &fromDuplicate, -1, 0);
    if (e) return e;

    char *s = mallocWithError((power + 2) * sizeof(char));
    s[0] = '0' + 1;
    int i = 1;
    for (; i < power + 1; i++) {
        s[i] = '0' + 0;
    }
    s[i] = '\0';
    bignum temp = bignum_default;
    e = initialize_bignum(&temp, s);
    if (e) return e;
    e = smultiply(&fromDuplicate, &temp, &fromDuplicate);
    if (e) return e;
    e = copyBignum(&fromDuplicate, to, -1, 0);
    if (e) return e;
    freeMallocSpace(&fromDuplicate);
    return 0;
}

int sdivide(bignum *b1, bignum *b2, bignum *ans, int precision, int roundOffLastDigit) {
    int e = 0;
    if (isZero(b1)) {
        char str[2];
        str[0] = '0'; // don't remove 0 from here
        str[1] = '\0';
        e = initialize_bignum(ans, str);
        if (e) return e;
        return 0;
    }

    if (isZero(b2)) {
        return 4;
    }
    // b1 is dividend and b2 is divisor
    bignum dividend = bignum_default;
    e = copyBignum(b1, &dividend, -1, 0);
    if (e) return e;
    bignum divisor = bignum_default;
    e = copyBignum(b2, &divisor, -1, 0);
    if (e) return e;

    int ansSign;
    if (dividend.sign == divisor.sign)
        ansSign = PLUS;
    else ansSign = MINUS;

    bignum tempans = bignum_default;
    // making dividend->decimal and divisor->decimal 0
    int extradecimal = b1->decimal - b2->decimal;
    if (extradecimal > 0) {
        e = multiplyBy10Pow(&divisor, &tempans, extradecimal);
        if (e) return e;
        e = copyBignum(&tempans, &divisor, -1, 0);
        if (e) return e;
        dividend.sign = PLUS;
        divisor.sign = PLUS;
        dividend.decimal = 0;
        divisor.decimal = 0;
    } else {
        extradecimal = -1 * extradecimal;
        e = multiplyBy10Pow(&dividend, &tempans, extradecimal);
        if (e) return e;
        e = copyBignum(&tempans, &dividend, -1, 0);
        if (e) return e;
        dividend.sign = PLUS;
        divisor.sign = PLUS;
        dividend.decimal = 0;
        divisor.decimal = 0;
    }
    int tempPrecision = roundOffLastDigit ? precision + 1 : precision;
    e = multiplyBy10Pow(&dividend, &tempans, tempPrecision);
    if (e) return e;
    e = copyBignum(&tempans, &dividend, -1, 0);
    if (e) return e;
    bignum temp = bignum_default;
    bignum subtractionResult = bignum_default;
    int dividendSize = dividend.size; // because it changes while division
    char *ansStr = mallocWithError((dividendSize + 1) * sizeof(char));
    if (ansStr == NULL) return 1;
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
            e = initialize_bignum(&temp, str);
            if (e) return e;
            e = smultiply(&divisor, &temp, &temp);
            if (e) return e;
            e = multiplyBy10Pow(&temp, &temp, dividendSize - totalCounter);
            if (e) return e;
            e = usub(&dividend, &temp, &subtractionResult);
            if (e) return e;
            if (subtractionResult.sign == MINUS) {
                str[0] = '0' + i; // don't remove 0 from here
                str[1] = '\0';
                e = initialize_bignum(&temp, str);
                if (e) return e;
                e = smultiply(&divisor, &temp, &temp);
                if (e) return e;
                e = multiplyBy10Pow(&temp, &temp, dividendSize - totalCounter);
                if (e) return e;
                e = usub(&dividend, &temp, &dividend);
                if (e) return e;
                break;
            }
        }
        ansStr[totalCounter - 1] = '0' + i;
    }
    ansStr[totalCounter - 1] = '\0';

    if (roundOffLastDigit == 1) {
        e = initialize_bignum(&temp, ansStr);
        if (e) return e;
        temp.decimal = precision + 1;
        e = copyBignum(&temp, ans, temp.size - 1, 1); // rounding off one extra precision made
        if (e) return e;
    } else {
        e = initialize_bignum(ans, ansStr);
        if (e) return e;
    }
    ans->decimal = precision;
    ans->sign = ansSign;
    fixLeadingZero(ans);
    freeMallocSpace(&tempans);
    freeMallocSpace(&temp);
    freeMallocSpace(&divisor);
    freeMallocSpace(&subtractionResult);
    freeMallocSpace(&dividend);
    return 0;
}


int generateReciprocal (bignum *b1, bignum *ans, int precision, int roundOffLastDigit) {
    int e = 0;
    bignum one = bignum_default;
    e = generateOne(&one);
    if (e) return e;
    e = sdivide(&one, b1, ans, precision, roundOffLastDigit);
    if (e) return e;
    freeMallocSpace(&one);
    return 0;
}

int usquareRoot(bignum *b1, bignum *ans, int precision, int roundOffLastDigit) {
    int e = 0;
    if (isZero(b1)) {
        char str[2];
        str[0] = '0'; // don't remove 0 from here
        str[1] = '\0';
        e = initialize_bignum(ans, str);
        if (e) return e;
        return 0;
    }

    // making a copy in dividend and making decimal places 0 and making a 0 initial divisor
    bignum divisor = bignum_default;
    generateEmpty(&divisor);
    bignum dividend = bignum_default;
    if (b1->decimal % 2 != 0) {
        e = copyBignum(b1, &dividend, b1->size + 1, 0);
        if (e) return e;
    } else {
        e = copyBignum(b1, &dividend, -1, 0);
        if (e) return e;
    }

    int tempPrecision = roundOffLastDigit ? (2 * precision - dividend.decimal + 2) : (2 * precision - dividend.decimal);
    int ansDecimal = precision;
    int ansSign = dividend.sign;
    dividend.decimal = 0;
    bignum tempans = bignum_default;
    e = multiplyBy10Pow(&dividend, &tempans, tempPrecision);
    if (e) return e;
    e = copyBignum(&tempans, &dividend, -1, 0);
    if (e) return e;
    fixLeadingZero(&dividend);

    bignum temp = bignum_default;
    bignum tempmult = bignum_default;
    bignum subtractionResult = bignum_default;

    int dividendSize = dividend.size; // because it changes while division
    char *ansStr = mallocWithError((dividendSize + 1) * sizeof(char));
    if (ansStr == NULL) return 1;

    int totalCounter;
    int strCounter = 0;
    if (dividend.size % 2 == 0) {
        totalCounter = 2;
    } else {
        totalCounter = 1;
    }
    for (; totalCounter <= dividendSize; totalCounter += 2) {
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

            e = initialize_bignum(&temp, str);
            if (e) return e;
            e = appendIgnoringDecimal(&divisor, &temp, &tempmult);
            if (e) return e;
            e = smultiply(&tempmult, &temp, &tempmult);
            if (e) return e;
            e = multiplyBy10Pow(&tempmult, &tempmult, dividendSize - totalCounter);
            if (e) return e;
            e = usub(&dividend, &tempmult, &subtractionResult);
            if (e) return e;

            if (subtractionResult.sign == MINUS) {
                str[0] = '0' + i; // don't remove 0 from here
                str[1] = '\0';
                e = initialize_bignum(&temp, str);
                if (e) return e;
                e = appendIgnoringDecimal(&divisor, &temp, &divisor);
                if (e) return e;
                e = smultiply(&divisor, &temp, &tempmult);
                if (e) return e;
                e = uadd(&divisor, &temp, &divisor, 0, 1);
                if (e) return e;
                e = multiplyBy10Pow(&tempmult, &tempmult, dividendSize - totalCounter);
                if (e) return e;
                e = usub(&dividend, &tempmult, &dividend);
                if (e) return e;
                break;
            }
        }
        ansStr[strCounter] = '0' + i;
        strCounter += 1;
    }
    ansStr[strCounter] = '\0';

    if (roundOffLastDigit == 1) {
        e = initialize_bignum(&temp, ansStr);
        if (e) return e;
        temp.decimal = precision + 1;
        e = copyBignum(&temp, ans, temp.size - 1, 1); // rounding off one extra precision made
        if (e) return e;
    } else {
        e = initialize_bignum(ans, ansStr);
        if (e) return e;
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
    return 0;
}

int powerHelper(bignum *b1, bignum *b2, bignum *ans, int precision) {
    int e = 0;
    if (b2->decimal != 0) {
        return 5;
    }
    if (isZero(b2)) {
        e = generateOne(ans);
        return e;
    }
    if (isOne(b2)) {
        e = copyBignum(b1, ans, -1, 0);
        return e;
    }

    if (b2->size > 0) {
        int lsb = b2->revdigits[0] - '0';
        if (lsb % 2 == 0) {
            char *s = mallocWithError((2) * sizeof(char));
            if (s == NULL) return 1;
            s[0] = '2';
            s[1] = '\0';
            bignum temp = bignum_default;
            e = initialize_bignum(&temp, s);
            if (e) return e;
            e = sdivide(b2, &temp, b2, 0, 0);
            if (e) return e;
            e = powerHelper(b1, b2, ans, precision);
            if (e) return e;
            e = smultiply(ans, ans, ans);
            if (e) return e;
        } else {
            char *s = mallocWithError((2) * sizeof(char));
            if (s == NULL) return 1;
            s[0] = '2';
            s[1] = '\0';
            bignum temp = bignum_default;
            e = initialize_bignum(&temp, s);
            if (e) return e;
            e = sdivide(b2, &temp, b2, 0, 0);
            if (e) return e;
            e = powerHelper(b1, b2, ans, precision);
            if (e) return e;
            e = smultiply(ans, ans, ans);
            if (e) return e;
            e = smultiply(ans, b1, ans);
            if (e) return e;
        }
    }
    return 0;
}

int absolute(bignum *bi, bignum *ans) {
    int e = copyBignum(bi, ans, -1, 0);
    ans->sign = PLUS;
    return e;
}

int spower(bignum *b1, bignum *b2, bignum *ans, int precision, int roundingOff) {

    bignum b1Duplicate = bignum_default; // in case b1 and ans are same
    int e = copyBignum(b1, &b1Duplicate, -1, 0);
    if (e) return e;
    bignum b2Duplicate = bignum_default; // in case b2 and ans are same
    e = copyBignum(b2, &b2Duplicate, -1, 0);
    if (e) return e;
    freeMallocSpace(ans);

    int anssign = PLUS;
    if (b2Duplicate.size > 0) {
        int lsb = b2Duplicate.revdigits[0] - '0';
        if (lsb % 2 == 0) {
            anssign = PLUS;
        } else {
            anssign = b1Duplicate.sign;
        }
    }
    if (b2Duplicate.sign == MINUS) {
        bignum temp = bignum_default;
        e = generateOne(&temp);
        if (e) return e;
        e = sdivide(&temp, &b1Duplicate, &b1Duplicate, 2 * precision, 0);
        if (e) return e;
    }
    e = powerHelper(&b1Duplicate, &b2Duplicate, ans, precision);
    if (e) return e;
    if (ans->decimal > (precision)) {
        bignum temp2 = bignum_default;
        e = copyBignum(ans, &temp2, ans->size - ans->decimal + precision, roundingOff);
        if (e) return e;
        e = copyBignum(&temp2, ans, -1, 0);
        if (e) return e;
        freeMallocSpace(&temp2);
    }
    ans->sign = anssign;
    freeMallocSpace(&b1Duplicate);
    freeMallocSpace(&b2Duplicate);
    return 0;
}

int ufactorial(bignum *bi, bignum *ans) {

    int e = 0;
    bignum one = bignum_default;
    e = generateOne(&one);
    if (e) return e;
    bignum biDuplicate = bignum_default; // in case from and to are same
    e = copyBignum(bi, &biDuplicate, -1, 0);
    if (e) return e;
    freeMallocSpace(ans);

    e = generateOne(ans);
    if (e) return e;
    if (biDuplicate.decimal > 0) {
        return 6;
    }

    while(!(isZero(&biDuplicate))) {
        e = smultiply(ans, &biDuplicate, ans);
        if (e) return e;
        e = ssub(&biDuplicate, &one, &biDuplicate);
        if (e) return e;
    }

    freeMallocSpace(&biDuplicate);
    freeMallocSpace(&one);
    return 0;
}

void string_bignum(bignum *bi, char *output) {
    int c = 0;
    if (bi->sign == MINUS) {
        output[c] = '-';
        c += 1;
    }
    for (int i = bi->size - 1; i >= 0; i--) {
        output[c] = bi->revdigits[i];
        c += 1;
        if (i == bi->decimal && i != 0) {
            output[c] = '.';
            c += 1;
        }
    }
    output[c] = '\n';
    output[c + 1] = '\0';
}

void print_bignum(bignum *bi) {
    if (bi->sign == MINUS) {
        printf("-");
    }
    for (int i = bi->size - 1; i >= 0; i--) {
        printf("%c", bi->revdigits[i]);
        if (i == bi->decimal && i != 0) printf(".");
    }
    printf("\n");
}


void parseSpace(char* cmd, char** argv) {
    int len = strlen(cmd);
    if (cmd[len - 1] == '\n') {
        len--;
        cmd[len] = '\0'; // so as to avoid getting \n
    }

    char* str = strtok(cmd," ");
    int i = 0;
    while (str != NULL) {
        argv[i] = str;
        str = strtok(NULL," ");
        i += 1;
    }
    argv[i] = NULL;
}

void execute(char* tokens[], char* output, int precision, int roundingOff) {

    bignum b1 = bignum_default;
    bignum b2 = bignum_default;
    bignum ans = bignum_default;

    if (strcmp(tokens[0], "ADD") == 0) {
        initialize_bignum(&b1, tokens[1]);
        initialize_bignum(&b2, tokens[2]);
        sadd(&b1, &b2, &ans);
    } else if(strcmp(tokens[0], "SUB") == 0) {
        initialize_bignum(&b1, tokens[1]);
        initialize_bignum(&b2, tokens[2]);
        ssub(&b1, &b2, &ans);
    } else if(strcmp(tokens[0], "MUL") == 0) {
        initialize_bignum(&b1, tokens[1]);
        initialize_bignum(&b2, tokens[2]);
        smultiply(&b1, &b2, &ans);
    } else if(strcmp(tokens[0], "DIV") == 0) {
        initialize_bignum(&b1, tokens[1]);
        initialize_bignum(&b2, tokens[2]);
        sdivide(&b1, &b2, &ans, 20, 0);
    } else if(strcmp(tokens[0], "ABS") == 0) {
        initialize_bignum(&b1, tokens[1]);
        absolute(&b1, &ans);
    } else if(strcmp(tokens[0], "POW") == 0) {
        initialize_bignum(&b1, tokens[1]);
        initialize_bignum(&b2, tokens[2]);
        spower(&b1, &b2, &ans, 20, 0);
    } else if(strcmp(tokens[0], "SQRT") == 0) {
        initialize_bignum(&b1, tokens[1]);
        usquareRoot(&b1, &ans, 20, 0);
    } else {
        printf("error: invalid line in input file");
    }

    if (ans.decimal > (precision)) {
        bignum temp2 = bignum_default;
        copyBignum(&ans, &temp2, ans.size - ans.decimal + precision, roundingOff);
        copyBignum(&temp2, &ans, -1, 0);
        freeMallocSpace(&temp2);
    }

    fixLeadingZero(&ans);
    fixTrailingZero(&ans);
    string_bignum(&ans, output);
    freeMallocSpace(&b1);
    freeMallocSpace(&b2);
    freeMallocSpace(&ans);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: pass arguments of input file and output file");
        exit(1);
    }
    FILE *outputFilePointer ;
    char dataToBeWritten[MAXSIZE];
    outputFilePointer = fopen(argv[2], "w") ;

    FILE *inputFilePointer;
    char dataToBeRead[MAXSIZE];
    char* tokens[30];
    inputFilePointer = fopen(
            argv[1],
            "r");
    if (inputFilePointer == NULL || outputFilePointer == NULL) {
        printf("error: input file failed to open");
    } else {
         while (fgets(dataToBeRead, MAXSIZE, inputFilePointer) != NULL) {
            // Print the dataToBeRead
            // printf("%s", dataToBeRead);
            parseSpace(dataToBeRead, tokens);
            execute(tokens, dataToBeWritten, 20, 0);
            printf(dataToBeWritten);
            // fputs(dataToBeWritten, outputFilePointer);
        }
        fclose(inputFilePointer);
    }
//
//
//    absolute(&b2);
//

//    bignum b1 = bignum_default;
//    bignum b2 = bignum_default;
//    bignum ans = bignum_default;
//    initialize_bignum(&b1, "1000");
//    initialize_bignum(&b2, "0");
//    print_bignum(&b1);
//    print_bignum(&b2);
//    ufactorial(&b1, &ans);
//    // sdivide(&b1, &b2, &ans, 20, 0);
//    // usquareRoot(&b1, &ans, 20, 0);
//    // smultiply(&b1, &b2, &ans);
//    // spower(&b1, &b2, &ans, 20, 0);
//    print_bignum(&ans);
    return 0;
}

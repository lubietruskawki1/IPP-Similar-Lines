#include "parse-text.h"
#include "structs.h"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <float.h>

#define INITIAL_LINE_BUFFER_SIZE 32
#define MIN_ASCII_CODE 33
#define MAX_ASCII_CODE 126
#define NUMBER_SIGN '#'
#define ZERO '0'
#define PLUS '+'
#define MINUS '-'
#define X 'X'
#define INITIAL_AMOUNT_OF_NUMBERS 4
#define INITIAL_AMOUNT_OF_NON_NUMBERS 4
#define WHITE_SPACES " \t\n\v\f\r"
#define NULL_TERMINATOR '\0'
#define STRING_0X "0X"

/* Funkcja powiększająca dwuktronie obszar pamięci tekstu, tzn. powiększająca
   ilość wierszy, a potem zwiększająca obszar pamięci tablicy wierszy. */
static void doubleTheAmountOfLines(Text *text) {
    text->amountOfLines = increase(text->amountOfLines);
    text->arrayOfLines = realloc(text->arrayOfLines,
                                        (size_t) (text->amountOfLines) * sizeof(Line));
    if (text->arrayOfLines == NULL) {
        exit(1);
    }
}

/* Funkcja powiększająca w miarę potrzeby dwuktronie obszar pamięci tekstu. */
static void doubleTheAmountOfLinesIfNecessary(Text *text, int i) {
    if (i + 1 == text->amountOfLines) {
        assert(i < INT_MAX);
        doubleTheAmountOfLines(text);
    }
}

/* Funkcja inicjalizująca nowy wiersz o indeksie i w tablicy wierszy tekstu. */
static void initializeLine(Text *text, int i) {
    text->arrayOfLines[i].correct = true;
    text->arrayOfLines[i].numbers.amount = INITIAL_AMOUNT_OF_NUMBERS;
    text->arrayOfLines[i].numbers.array = malloc(
                                        (size_t) INITIAL_AMOUNT_OF_NUMBERS * sizeof(long double));
    text->arrayOfLines[i].nonNumbers.amount = INITIAL_AMOUNT_OF_NON_NUMBERS;
    text->arrayOfLines[i].nonNumbers.array = malloc(
                                        (size_t) INITIAL_AMOUNT_OF_NON_NUMBERS * sizeof(char*));
}

static bool containsAnIllegalCharacter(char *word) {
    while (*word) {
        if (*word < MIN_ASCII_CODE || *word > MAX_ASCII_CODE) {
            return true;
        }
        word++;
    }
    return false;
}

static void convertCharactersToUppercase(char *word) {
    while (*word) {
        *word = (char) toupper(*word);
        word++;
    }
}

/* Funkcja sprawdzająca, czy dany wyraz to "0X". */
static bool is0X(char *word) {
    int result = strcmp(word, STRING_0X);
    return (result == 0);
}

/* Funkcja sprawdzająca, czy wartość danej liczby całkowitej w systemie
   szesnastkowym, ósemkowym lub dziesiętnym nie przekracza maksymalnej
   dozwolonej wartości. */
static bool isNotTooBig(long double value) {
    return (value <= ULONG_MAX);
}

/* Funkcja sprawdzająca, czy wartość danej liczby całkowitej w systemie
   dziesiętnym nie przekracza minimalnej dozwolonej wartości. */
static bool isNotTooSmall(long double value) {
    return (value >= LONG_MIN);
}

/* Funkcja sprawdzająca, czy dane słowo rozpoczyna się prefiksem
   "0X", "+0X" bądź "-0X". */
static bool hasHexadecimalPrefix(char *word) {
    if (word[0] == ZERO && word[1] == X) {
        return true;
    }
    if ((word[0] == PLUS || word[0] == MINUS) && word[1] == ZERO && word[2] == X) {
        return true;
    }
    return false;
}

/* Funkcja sprawdzająca, czy dane słowo jest liczbę całkowitą nieujemną
   zapisaną szesnastkowo. */
static bool isCorrectHexadecimalNumber(char *word) {
    /* Specjalny przypadek dla słowa "0X". */
    if (is0X(word)) {
        return true;
    }

    char *endPtrHex = word;
    unsigned long long int valueHex = strtoull(word, &endPtrHex, 16);
    return (!(word == endPtrHex || *endPtrHex != NULL_TERMINATOR)
                                            && word[0] == ZERO && isNotTooBig(valueHex));
}

/* Funkcja sprawdzająca, czy dane słowo jest liczbę całkowitą nieujemną
   zapisaną ósemkowo. */
static bool isCorrectOctalNumber(char *word, unsigned long long int valueOct, char *endPtrOct) {
    return (!(word == endPtrOct || *endPtrOct != NULL_TERMINATOR)
                                            && word[0] == ZERO && isNotTooBig(valueOct));
}

/* Funkcja sprawdzająca, czy dane słowo jest liczbę całkowitą
   zapisaną dziesiętnie. */
static bool isCorrectDecimalNumber(char *word, long double value) {
    char *endPtrDec = word;
    long long int valueDec = strtoll(word, &endPtrDec, 10);
    return (!(word == endPtrDec || *endPtrDec != NULL_TERMINATOR)
                                    && isNotTooBig(value) && isNotTooSmall(valueDec));
}

static void doubleTheAmountOfNumbers(Numbers *numbers) {
    numbers->amount = increase(numbers->amount);
    numbers->array = realloc(numbers->array,
                             (size_t) (numbers->amount) * sizeof(long double));
    if (numbers->array == NULL) {
        exit(1);
    }
}

/* Funkcja powiększająca w miarę potrzeby dwuktronie obszar pamięci liczb
   w wierszu o indeksie i w tekście. */
static void doubleTheAmountOfNumbersIfNecessary(Text *text, int i, int indexNumbers) {
    if (indexNumbers == text->arrayOfLines[i].numbers.amount) {
        assert(i < INT_MAX);
        doubleTheAmountOfNumbers(&text->arrayOfLines[i].numbers);
    }
}

/* Funkcja dodająca wartość do tablicy liczb wiersza o indeksie i. */
static void insertANumber(Text *text, int i, int *indexNumbers, long double value) {
    doubleTheAmountOfNumbersIfNecessary(text, i, *indexNumbers);
    text->arrayOfLines[i].numbers.array[*indexNumbers] = value;
    *indexNumbers += 1;
}

static void doubleTheAmountOfNonNumbers(NonNumbers *nonNumbers) {
    nonNumbers->amount = increase(nonNumbers->amount);
    nonNumbers->array = realloc(nonNumbers->array,
                                (size_t) (nonNumbers->amount) * sizeof(char*));
    if (nonNumbers->array == NULL) {
        exit(1);
    }
}

/* Funkcja powiększająca w miarę potrzeby dwuktronie obszar pamięci nieliczb
   w wierszu o indeksie i w tekście. */
static void doubleTheAmountOfNonNumbersIfNecessary(Text *text, int i, int indexNonNumbers) {
    if (indexNonNumbers == text->arrayOfLines[i].nonNumbers.amount) {
        assert(i < INT_MAX);
        doubleTheAmountOfNonNumbers(&text->arrayOfLines[i].nonNumbers);
    }
}

/* Funkcja dodająca słowo do tablicy nieliczb wiersza o indeksie i. */
static void insertANonNumber(Text *text, int i, int *indexNonNumbers, char *word) {
    doubleTheAmountOfNonNumbersIfNecessary(text, i, *indexNonNumbers);
    text->arrayOfLines[i].nonNumbers.array[*indexNonNumbers] =
                                                        malloc(sizeof(char) * strlen(word) + 1);
    /* W mallocu dodajemy +1, gdyż funkcja strcpy dodaje na koniec stringa '\0'. */
    strcpy(text->arrayOfLines[i].nonNumbers.array[*indexNonNumbers], word);
    *indexNonNumbers += 1;
}

/* Funkcja porównująca liczby w qsorcie. */
static int compareNumbers(const void *a, const void *b) {
    const long double ia = *(const long double *)a;
    const long double ib = *(const long double *)b;
    if (ia < ib) {
        return -1;
    } else if (ia == ib) {
        return 0;
    } else {
        return 1;
    }
}

/* Funkcja porównująca nieliczby w qsorcie. */
static int compareNonNumbers(const void *a, const void *b) {
    const char* ia = *(const char **)a;
    const char* ib = *(const char **)b;
    return strcmp(ia, ib);
}

void parseText(Text *text) {
    char *lineBuffer;
    size_t lineBufferSize = INITIAL_LINE_BUFFER_SIZE;
    ssize_t numberOfCharacters;
    
    lineBuffer = malloc(lineBufferSize * sizeof(char));

    if (lineBuffer == NULL) {
        exit(1);
    }

    char *word;
    int i = 1;

    /* Wartość mówiąca o tym, czy dany wiersz powinien być ignorowany, bądź
       zawiera niedozwolony znak. */
    bool incorrectLine;

    int indexNumbers, indexNonNumbers;
    
    while ((numberOfCharacters = getline(&lineBuffer, &lineBufferSize, stdin)) != -1) {

        if (lineBuffer == NULL) {
            exit(1);
        }
        
        doubleTheAmountOfLinesIfNecessary(text, i);
        
        incorrectLine = false;
        
        initializeLine(text, i);

        indexNumbers = 0;
        indexNonNumbers = 0;


        if (numberOfCharacters != (ssize_t) strlen(lineBuffer)) {
            fprintf(stderr, "ERROR %d\n", i);
            incorrectLine = true;
        }

        word = strtok(lineBuffer, WHITE_SPACES);

        if (word == NULL || lineBuffer[0] == NUMBER_SIGN || incorrectLine) {
            /* Pusty wiersz lub komentarz lub niepoprawny wiersz. */
            text->arrayOfLines[i].correct = false;
        } else {

            while (word != NULL && !incorrectLine) {

                if (containsAnIllegalCharacter(word)) {
                    fprintf(stderr, "ERROR %d\n", i);
                    incorrectLine = true;
                    text->arrayOfLines[i].correct = false;
                    break;
                }

                convertCharactersToUppercase(word);

                char *endPtr = word;
                errno = 0;
                long double value = strtold(word, &endPtr);

                if ((word == endPtr || *endPtr != '\0' || isnan(value)
                    || ((value == LDBL_MAX || value == LDBL_MIN) && errno == ERANGE))
                        && !is0X(word)) {
                    /* Słowo jest nieliczbą. */
                    insertANonNumber(text, i, &indexNonNumbers, word);

                } else {

                    if (hasHexadecimalPrefix(word)) {

                        if (isCorrectHexadecimalNumber(word)) {
                            /* Słowo jest poprawną liczbą w systemie szesnastkowym. */
                            insertANumber(text, i, &indexNumbers, value);
                        } else {
                            /* Słowo jest nieliczbą. */
                            insertANonNumber(text, i, &indexNonNumbers, word);
                        }

                    } else {

                        /* Osobno przetworzone słowo, gdyż funkcja strtold rozpoznaje system
                           dziesiątkowy i szesnastkowy, ale ósemkowego nie. */
                        char *endPtrOct = word;
                        unsigned long long int valueOct = strtoull(word, &endPtrOct, 8);

                        if (isCorrectOctalNumber(word, valueOct, endPtrOct)) {
                            /* Słowo jest poprawną liczbą w systemie ósemkowym. */
                            insertANumber(text, i, &indexNumbers, valueOct);

                        } else if (isCorrectDecimalNumber(word, value)) {
                            /* Słowo jest poprawną liczbą w systemie dziesiętnym. */
                            insertANumber(text, i, &indexNumbers, value);

                        } else {
                            /* Słowo jest liczbą zmiennoprzecinkową. */
                            //printf("zmienno %s %Lf\n",word,value);
                            insertANumber(text, i, &indexNumbers, value);
                        }
                    }
                }
                word = strtok(NULL, WHITE_SPACES);
            }
        }

        text->arrayOfLines[i].numbers.amount = indexNumbers;
        text->arrayOfLines[i].nonNumbers.amount = indexNonNumbers;

        if (text->arrayOfLines[i].numbers.amount != 0) {
            /* Sortujemy tablicę liczb w danym wierszu. */
            qsort(text->arrayOfLines[i].numbers.array,
                  (size_t) text->arrayOfLines[i].numbers.amount,
                        sizeof(long double), compareNumbers);
        }

        if (text->arrayOfLines[i].nonNumbers.amount != 0) {
            /* Sortujemy tablicę nieliczb w danym wierszu. */
            qsort(text->arrayOfLines[i].nonNumbers.array,
                  (size_t) text->arrayOfLines[i].nonNumbers.amount,
                        sizeof(char *), compareNonNumbers);
        }

        ++i;
    }

    text->amountOfLines = i - 1;

    free(lineBuffer);
}

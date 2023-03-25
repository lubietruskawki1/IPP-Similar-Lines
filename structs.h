#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

/* Struktura przechowująca liczby w danym wierszu. */
typedef struct Numbers {
    long double *array;
    int amount;
} Numbers;

/* Struktura przechowująca nieliczby w danym wierszu. */
typedef struct NonNumbers {
    char **array;
    int amount;
} NonNumbers;

/* Struktura przechowująca wiersz. */
typedef struct Line {
    Numbers numbers;
    NonNumbers nonNumbers;
    bool correct; // Czy wiersz powinien być zignorowany lub zawiera niedozwolony znak.
} Line;

/* Struktura przechowująca tekst podzielony na wiersze. */
typedef struct Text {
    Line *arrayOfLines;
    int amountOfLines;
} Text;

/* Struktura przechowująca numery podobnych wierszy w grupie. */
typedef struct SimilarLines {
    int *array;
    int amount;
    int index; // Indeks, na który ostatnio dodaliśmy liczbę w tablicy.
} SimilarLines;

/* Struktura przechowująca grupę podobnych wierszy. */
typedef struct Group {
    Numbers numbers; // Liczby w wierszach tej grupy.
    NonNumbers nonNumbers; // Nieliczby w wierszach tej grupy.
    SimilarLines similarLines; // Numery wierszy należących do tej grupy.
} Group;

/* Struktura przechowująca grupy podobnych wierszy. */
typedef struct Groups {
    Group *array;
    int amount;
} Groups;

#endif //STRUCTS_H

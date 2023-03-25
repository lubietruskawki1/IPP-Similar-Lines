#ifndef PARSE_TEXT_H
#define PARSE_TEXT_H

#include "structs.h"

/* Funkcja zwracająca wartość wyrażenia 2n dla danej liczby n. */
static inline int increase(int n) {
    return 2 * n;
};

/* Funkcja parsująca dany tekst i wpisująca go do struktury Text. */
extern void parseText(Text *text);

#endif //PARSE_TEXT_H

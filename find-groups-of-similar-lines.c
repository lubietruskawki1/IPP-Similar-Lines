#include "find-groups-of-similar-lines.h"
#include "parse-text.h"
#include "structs.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#define INITIAL_AMOUNT_OF_SIMILAR_LINES 4

/* Funkcja powiększająca dwuktronie obszar pamięci dla grup, tzn. powiększająca
   ich ilość, a potem zwiększająca obszar pamięci tablicy grup. */
static void doubleTheAmountOfGroups(Groups *groups) {
    groups->amount = increase(groups->amount);
    groups->array = realloc(groups->array,
                            (size_t) (groups->amount) * sizeof(Group));
    if (groups->array == NULL) {
        exit(1);
    }
}

/* Funkcja powiększająca w miarę potrzeby dwuktronie obszar pamięci dla grup. */
static void doubleTheAmountOfGroupsIfNecessary(Groups *groups, int i) {
    if (i == groups->amount) {
        assert(i < INT_MAX);
        doubleTheAmountOfGroups(groups);
    }
}

/* Funkcja inicjalizująca nową grupę o indeksie i w tablicy grup. */
static void initializeGroup(Groups *groups, int i, Line line) {
    doubleTheAmountOfGroupsIfNecessary(groups, i);
    groups->array[i].numbers = line.numbers;
    groups->array[i].nonNumbers = line.nonNumbers;
    groups->array[i].similarLines.index = 0;
    groups->array[i].similarLines.amount = INITIAL_AMOUNT_OF_SIMILAR_LINES;
    groups->array[i].similarLines.array =
                                malloc((size_t) INITIAL_AMOUNT_OF_SIMILAR_LINES * sizeof(int));
}

/* Funkcja powiększająca dwuktronie obszar pamięci podobnych wierszy w grupie,
   tzn. powiększająca ich ilość, a potem zwiększająca obszar pamięci ich tablicy. */
static void doubleTheAmountOfSimilarLinesInAGroup(Group *group) {
    group->similarLines.amount = increase(group->similarLines.amount);
    group->similarLines.array = realloc(group->similarLines.array,
                                        (size_t) (group->similarLines.amount) * sizeof(int));
    if (group->similarLines.array == NULL) {
        exit(1);
    }
}

/* Funkcja powiększająca w miarę potrzeby dwuktronie obszar pamięci podobnych wierszy
   w grupie o indeksie i. */
static void doubleTheAmountOfSimilarLinesInAGroupIfNecessary(Groups *groups, int i, int index) {
    if (index == groups->array[i].similarLines.amount) {
        assert(index < INT_MAX);
        doubleTheAmountOfSimilarLinesInAGroup(&groups->array[i]);
    }
}

/* Funkcja dodająca indeks wiersza do tablicy podobnych wierszy
   w grupie o indeksie i. */
static void insertASimilarLine(Groups *groups, int i, int k) {
    int index = groups->array[i].similarLines.index;
    doubleTheAmountOfSimilarLinesInAGroupIfNecessary(groups, i, index);
    groups->array[i].similarLines.array[index] = k;
    groups->array[i].similarLines.index += 1;
}

void findGroupsOfSimilarLines(Text text, Groups *groups) {
    int i = 0; // Indeks tablicy grup.

    for (int k = 1; k <= text.amountOfLines; ++k) {

        Line line = text.arrayOfLines[k];

        /* Jeśli wiersz jest niepoprawny, to go ignorujemy. */
        if (line.correct) {
            
            if (i == 0) {
                /* Jeśli to pierwszy poprawny wiersz to tworzymy pierwszą grupę
                   i dodajemy go tam. */
                initializeGroup(groups, i, line);
                insertASimilarLine(groups, i, k);
                ++i;
                
            } else {
                /* W przeciwnym przypadku porównujemy go z wierszami w istniejących grupach. */
                bool similar = true; // Czy ten wiersz jest podobny do wierszy w danej grupie.
                bool found = false; // Czy znaleziono odpowiednią istniejącą grupę.

                for (int j = 0; j < i && !found; ++j) {

                    Numbers *numbers = &groups->array[j].numbers;
                    NonNumbers *nonNumbers = &groups->array[j].nonNumbers;

                    /* Porównujemy, czy zgadza się ilość liczb. */
                    if ((*numbers).amount != line.numbers.amount) {
                        similar = false;

                    /* Porównujemy, czy zgadza się ilość nieliczb. */
                    } else if ((*nonNumbers).amount != line.nonNumbers.amount) {
                        similar = false;

                    } else {

                        /* Porównujemy, czy liczby się zgadzają. */
                        for (int l = 0; l < (*numbers).amount && similar; ++l) {
                            if ((*numbers).array[l] != line.numbers.array[l]) {
                                similar = false;
                            }
                        }

                        /* Porównujemy, czy nieliczby się zgadzają. */
                        for (int l = 0; l < (*nonNumbers).amount && similar; ++l) {
                            int result = strcmp((*nonNumbers).array[l], line.nonNumbers.array[l]);
                            if (result != 0) {
                                similar = false;
                            }
                        }

                    }

                    if (similar) {
                        /* Znaleziono odpowiednią grupę. */
                        found = true;
                        insertASimilarLine(groups, j, k);
                    } else {
                        similar = true;
                    }
                }

                if (!found) {
                    /* Nie istnieje odpowiednia grupa, trzeba ją stworzyć. */
                    initializeGroup(groups, i, line);
                    insertASimilarLine(groups, i, k);
                    ++i;
                }
            }
        }
    }

    groups->amount = i;

    for (int i = 0; i < groups->amount; ++i) {
        groups->array[i].similarLines.amount = groups->array[i].similarLines.index;
    }
}
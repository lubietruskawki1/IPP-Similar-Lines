#include "print-groups-and-free-memory.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>

void printGroupsOfSimilarLines(Groups groups) {
    for (int i = 0; i < groups.amount; ++i) {
        Group group = groups.array[i];
        /* Pierwszy indeks wiersza printujemy osobno,
           by uniknąć spacji na końcu linijki. */
        printf("%d",group.similarLines.array[0]);
        for (int j = 1; j < group.similarLines.amount; ++j) {
            printf(" %d",group.similarLines.array[j]);
        }
        printf("\n");
    }
}

void freeMemory(Text *text, Groups *groups) {
    /* Zwalniamy pamięć przechowywaną przez tekst. */
    for (int i = 1; i <= text->amountOfLines; ++i) {

        /* Pamięć zajęta przez liczby. */
        free(text->arrayOfLines[i].numbers.array);

        /* Pamięć zajęta przez nieliczby. */
        for (int j = 0; j < text->arrayOfLines[i].nonNumbers.amount; ++j) {
            free(text->arrayOfLines[i].nonNumbers.array[j]);
        }
        free(text->arrayOfLines[i].nonNumbers.array);
    }
    free(text->arrayOfLines);

    /* Zwalniamy pamięć przechowywaną przez grupy. */
    for (int i = 0; i < groups->amount; ++i) {
        free(groups->array[i].similarLines.array);
    }
    free(groups->array);
}

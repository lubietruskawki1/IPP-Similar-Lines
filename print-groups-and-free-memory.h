#ifndef PRINT_GROUPS_AND_FREE_MEMORY_H
#define PRINT_GROUPS_AND_FREE_MEMORY_H

#include "structs.h"

/* Funkcja wypisująca grupy podobnych wierszy. */
extern void printGroupsOfSimilarLines(Groups groups);

/* Funkcja zwalniająca pamięć. */
extern void freeMemory(Text *text, Groups *groups);

#endif //PRINT_GROUPS_AND_FREE_MEMORY

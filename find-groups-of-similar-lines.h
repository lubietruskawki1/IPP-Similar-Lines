#ifndef FIND_GROUPS_OF_SIMILAR_LINES_H
#define FIND_GROUPS_OF_SIMILAR_LINES_H

#include "structs.h"

/* Funkcja znajdująca w danym tekście grupy podobnych wierszy i wpisująca
   je do struktury Groups. */
extern void findGroupsOfSimilarLines(Text text, Groups *groups);

#endif //FIND_GROUPS_OF_SIMILAR_LINES_H

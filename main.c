#include "structs.h"
#include "parse-text.h"
#include "find-groups-of-similar-lines.h"
#include "print-groups-and-free-memory.h"
#include <stdlib.h>

#define INITIAL_AMOUNT_OF_LINES 4
#define INITIAL_AMOUNT_OF_GROUPS 4

int main() {
    Text text;
    text.arrayOfLines = malloc((unsigned) INITIAL_AMOUNT_OF_LINES * sizeof(Line));
    text.amountOfLines = INITIAL_AMOUNT_OF_LINES;

    if (text.arrayOfLines == NULL) {
        exit(1);
    }

    parseText(&text);

    Groups groups;
    groups.array = malloc((unsigned) INITIAL_AMOUNT_OF_GROUPS * sizeof(Group));
    groups.amount = INITIAL_AMOUNT_OF_GROUPS;

    if (groups.array == NULL) {
        exit(1);
    }

    findGroupsOfSimilarLines(text, &groups);

    printGroupsOfSimilarLines(groups);

    freeMemory(&text, &groups);

    return 0;
}

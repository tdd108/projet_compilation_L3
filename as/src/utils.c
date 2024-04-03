#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

int isnumber(char *str) {
    for (int i = 0; i < strlen(str); i++) 
        if (!isdigit(str[i])) return 0;
    return 1;
}
/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#include "ascii_text.h"
#include <stddef.h>

int stringcmp(char *a, char *b)
{
    if (a == b)
    {
        return 1;
    }
    if (a == NULL || b == NULL)
    {
        return 0;
    }

    while (*a != '\0' && *b != '\0')
    {
        if (*a != *b)
        {
            return 0;
        }
        ++a;
        ++b;
    }

    return (*a == '\0' && *b == '\0') ? 1 : 0;
}

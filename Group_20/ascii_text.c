/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#include "ascii_text.h"

int stringcmp(char *a, char *b)
{
     int i = 0;
  while (a[i] != '\0' && b[i] != '\0')

{
      if (a[i] != b[i])
    {
        return 0;
    }
      i++;
 }
if (a[i] == '\0' && b[i] == '\0')
{
  return 1;
}
return 0;

}

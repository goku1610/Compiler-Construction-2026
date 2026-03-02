/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static const char *MAIN_MENU = "What do you want to do:\n"
  "\t0. Exit\n"
   "\t1. Remove Comments\n"
    "\t2. Print Lexer Token List\n"
"\t3. Parse Code and Print Parse Tree\n"
  "\t4. Parse Code and Print Time Taken\n";

 int main(int argc, char *argv[])
  {
printf("(c) Both lexical and syntax analysis modules implemented\n");


if (argc != 3)
{
  printf("Usage: %s testcase.txt parsetreeOutFile.txt\n", argv[0]);
     return 1;

}

 char *in = argv[1];
char *out = argv[2];
  char *tmp = "cleanFile_tmp.txt";
table T;

 while (1)
{
   int choice = -1;
printf("%s", MAIN_MENU);
   printf("==> ");
 if (scanf("%d", &choice) != 1)

    {

      printf("Invalid input. Exiting.\n");
       return 1;

   }

  switch (choice)
 {
case 0:
   return 0;

case 1:

 {
    removeComments(in, tmp);
    FILE *fp = fopen(tmp, "r");
 if (fp == NULL)
  {
      printf("Unable to open generated comment-free file: %s\n\n",
    tmp);
         break;
      }

 int ch;
    while ((ch = fgetc(fp)) != EOF)
{
       putchar(ch);
}
   fclose(fp);
   remove(tmp);
if (ch != '\n')
 {
   putchar('\n');
}
 break;
  }


case 2:
{
     FILE *fp = fopen(in, "r");
 if (fp == NULL)
  {
 printf("Unable to open input file: %s\n\n", in);
  break;

     }


   getStream(fp);
fclose(fp);
printf("Total lexical errors: %d\n\n", getLexicalErrorCount());
break;
}

case 3:
 {
    parseTree *PT = parseInputSourceCode(in, T);

   if (PT == NULL)
   {
   printf("Unable to parse input source code.\n\n");
    break;
 }

 printParseTree(*PT, out);
printf("Parse tree written to %s\n\n", out);
 break;
}

case 4:
  {
clock_t st = clock();

parseTree *PT = parseInputSourceCode(in, T);
clock_t et = clock();
   (void)PT;

double cpu = (double)(et - st);
  double sec = cpu / CLOCKS_PER_SEC;
printf("total_CPU_time: %.0f\n", cpu);
   printf("total_CPU_time_in_seconds: %.6f\n\n", sec);
     break;
}



default:
   printf("Invalid choice. Try again.\n\n");

break;
}
}
  }

#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const char *MAIN_MENU =
    "What do you want to do:\n"
    "\t0. Exit\n"
    "\t1. Remove Comments\n"
    "\t2. Print Lexer Token List\n"
    "\t3. Parse Code and Print Parse Tree\n"
    "\t4. Parse Code and Print Time Taken\n";

int main(int argc, char *argv[]) {
  printf("(c) Both lexical and syntax analysis modules implemented\n");

  if (argc != 3) {
    printf("Usage: %s testcase.txt parsetreeOutFile.txt\n", argv[0]);
    return 1;
  }

  char *inputPath = argv[1];
  char *outputPath = argv[2];
  char *commentFreePath = "cleanFile_tmp.txt";
  table T;

  while (1) {
    int choice = -1;
    printf("%s", MAIN_MENU);
    printf("==> ");
    if (scanf("%d", &choice) != 1) {
      printf("Invalid input. Exiting.\n");
      return 1;
    }

    switch (choice) {
    case 0:
      return 0;

    case 1: {
      removeComments(inputPath, commentFreePath);
      FILE *cleanFile = fopen(commentFreePath, "r");
      if (cleanFile == NULL) {
        printf("Unable to open generated comment-free file: %s\n\n",
               commentFreePath);
        break;
      }

      int ch;
      while ((ch = fgetc(cleanFile)) != EOF) {
        putchar(ch);
      }
      fclose(cleanFile);
      remove(commentFreePath);
      if (ch != '\n') {
        putchar('\n');
      }
      break;
    }

    case 2: {
      FILE *fp = fopen(inputPath, "r");
      if (fp == NULL) {
        printf("Unable to open input file: %s\n\n", inputPath);
        break;
      }

      getStream(fp);
      fclose(fp);
      printf("Total lexical errors: %d\n\n", getLexicalErrorCount());
      break;
    }

    case 3: {
      parseTree *root = parseInputSourceCode(inputPath, T);
      if (root == NULL) {
        printf("Unable to parse input source code.\n\n");
        break;
      }

      printParseTree(root, outputPath);
      printf("Parse tree written to %s\n\n", outputPath);
      break;
    }

    case 4: {
      clock_t start_time = clock();
      parseTree *root = parseInputSourceCode(inputPath, T);
      clock_t end_time = clock();
      (void)root;

      double total_CPU_time = (double)(end_time - start_time);
      double total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
      printf("total_CPU_time: %.0f\n", total_CPU_time);
      printf("total_CPU_time_in_seconds: %.6f\n\n", total_CPU_time_in_seconds);
      break;
    }

    default:
      printf("Invalid choice. Try again.\n\n");
      break;
    }
  }
}

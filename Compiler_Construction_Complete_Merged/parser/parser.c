#include "parser/parser.h"
#include "parser/first_follow.h"
#include "parser/ll1_frontend.h"

#include <stdio.h>

FirstAndFollow ComputeFirstAndFollowSets(grammar G) {
  return computeFirstFollowSet(G);
}

void createParseTable(FirstAndFollow F, table *T) {
  if (T == NULL) {
    return;
  }
  buildParseTable(F, T);
}

void createParseTableStrict(FirstAndFollow F, table *T) {
  createParseTable(F, T);
}

parseTree *parseInputSourceCode(char *testcaseFile, table T) {
  if (testcaseFile == NULL) {
    return NULL;
  }

  FILE *fp = fopen(testcaseFile, "r");
  if (fp == NULL) {
    printf("Unable to open input file: %s\n", testcaseFile);
    return NULL;
  }

  grammar G = initializeGrammar();
  FirstAndFollow F = ComputeFirstAndFollowSets(G);
  buildParseTable(F, &T);
  parseTree *root = parseInputSourceCodeStream(T, F, G, fp);
  fclose(fp);
  return root;
}

void printParseTree(parseTree *PT, char *outfile) {
  if (PT == NULL || outfile == NULL) {
    return;
  }

  FILE *fp = fopen(outfile, "w");
  if (fp == NULL) {
    printf("Unable to open output file: %s\n", outfile);
    return;
  }

  printParseTreeInorder(PT, fp);
  fclose(fp);
}


#ifndef LL1_FRONTEND_H
#define LL1_FRONTEND_H
#include "core/grammar_model.h"
#include <stdio.h>

void createParseTable(FirstFollow F, table *T);

parseTree *parseInputSourceCode(table T, FirstFollow F, grammar G, FILE *fp);

void printParseTree(parseTree *PT, FILE *outfile);
void writeParseTreeDot(parseTree *PT, FILE *outfile);

FirstFollow computeFirstFollowSet(grammar G);
#endif

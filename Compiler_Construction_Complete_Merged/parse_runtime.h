/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/


#ifndef PARSE_RUNTIME_H
#define PARSE_RUNTIME_H
#include "grammar_model.h"
#include <stdio.h>


 void printParseTreeInorder(parseTree *PT, FILE *fp);
 void buildParseTable(FirstAndFollow ff, table *T);
FirstAndFollow computeFirstandFollowSets(grammar G);
parseTree *parseInputSourceCodeStream(table T, FirstAndFollow ff, grammar G,
                                      FILE *fp);

void writeParseTreeDot(parseTree *PT, FILE *fp);
#endif

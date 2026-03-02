/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/
#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

FirstAndFollow ComputeFirstAndFollowSets(grammar G);
void printParseTree(parseTree PT, char *outfile);
void createParseTableStrict(FirstAndFollow ff, table *T);
parseTree *parseInputSourceCode(char *infile, table T);
void createParseTable(FirstAndFollow ff, table *T);

#endif

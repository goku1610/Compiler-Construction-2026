#ifndef PARSER_H
#define PARSER_H

#include "parser/parserDef.h"

FirstAndFollow ComputeFirstAndFollowSets(grammar G);
void createParseTable(FirstAndFollow F, table *T);
void createParseTableStrict(FirstAndFollow F, table *T);

parseTree *parseInputSourceCode(char *testcaseFile, table T);
void printParseTree(parseTree *PT, char *outfile);

#endif

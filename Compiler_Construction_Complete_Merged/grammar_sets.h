/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#ifndef GRAMMAR_SETS_H
#define GRAMMAR_SETS_H
#include "grammar_model.h"


grammar initializeGrammar();
void followHelper(FirstAndFollow *ff, grammar_rule rule, NON_TERMINAL LHS,
 NON_TERMINAL **followDep, int *depCount);
void followDepAdd(NON_TERMINAL nt, NON_TERMINAL dep, NON_TERMINAL **followDep,
                  int *depCount);
void clearDependency(NON_TERMINAL nt, NON_TERMINAL **followDep, int *depCount,
                     FirstAndFollow *ff);
void computeFirstRec(FirstAndFollow *ff, NON_TERMINAL nt, grammar G, bool *fr);

 char *getNonTerminal(NON_TERMINAL nt);
 void firstUnion(FirstAndFollow *ff, NON_TERMINAL nt1, NON_TERMINAL nt2);
void followAdd(FirstAndFollow *ff, NON_TERMINAL nt, TOKEN_TYPE t);
#endif

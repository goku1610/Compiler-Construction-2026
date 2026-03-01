
#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H
#include "core/grammar_model.h"

grammar initializeGrammar();
void firstUnion(FirstFollow *ff, NON_TERMINAL nt1, NON_TERMINAL nt2);
char *getNonTerminal(NON_TERMINAL nt);
void computeFirstRec(FirstFollow *ff, NON_TERMINAL nt, grammar G,
                     bool *firstComputed);
void followAdd(FirstFollow *ff, NON_TERMINAL nt, TOKEN_TYPE t);
void followDepAdd(NON_TERMINAL nt, NON_TERMINAL a, NON_TERMINAL **followDep,
                  int *depCount);
void followHelper(FirstFollow *ff, grammar_rule rule, NON_TERMINAL LHS,
                  NON_TERMINAL **followDep, int *depCount);
void clearDependency(NON_TERMINAL nt, NON_TERMINAL **followDep, int *depCount,
                     FirstFollow *ff);
#endif

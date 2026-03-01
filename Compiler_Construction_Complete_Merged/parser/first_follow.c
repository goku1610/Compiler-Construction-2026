
#include "parser/first_follow.h"
#include "core/token_model.h"
#include "core/grammar_model.h"
#include <stdlib.h>

void firstUnion(FirstFollow *ff, NON_TERMINAL nt1, NON_TERMINAL nt2) {

  int nt1_next = ff->first_count[nt1];

  for (int i = 0; i < ff->first_count[nt2]; i++) {
    TOKEN_TYPE tok = ff->first[nt2][i];
    int j;
    for (j = 0; j < nt1_next; j++) {
      if (ff->first[nt1][j] == ff->first[nt2][i]) {
        break;
      }
    }
    if (j == nt1_next) {
      ff->first[nt1][nt1_next] = ff->first[nt2][i];
      nt1_next++;
    }
  }
  ff->first_count[nt1] = nt1_next;
}

void computeFirstRec(FirstFollow *ff, NON_TERMINAL nt, grammar G,
                     bool *firstComputed) {
  if (firstComputed[nt]) {
    return;
  }

  
  for (int rule_no = 0; rule_no < G.rule_count[nt]; rule_no++) {
    grammar_rule rule = G.rules[nt][rule_no];
    int j = 0;

    int old_count = ff->first_count[nt];
    
    while (j < rule.element_count) {

      
      if (rule.elements[j].terminal) {
        ff->first[nt][(ff->first_count[nt])++] = rule.elements[j].var.t;
        break;
      }

      NON_TERMINAL curr = rule.elements[j].var.nt;

      
      computeFirstRec(ff, curr, G, firstComputed);

      
      
      firstUnion(ff, nt, curr);

      
      if (!ff->first_has_epsillon[curr]) {
        break;
      }
      if (j == rule.element_count - 1) {
        ff->follow_rule[nt] = rule_no;
      }
      j++;
    }

    for (int i = old_count; i < ff->first_count[nt]; i++) {
      ff->rule_no[nt][i] = rule_no;
    }
  }

  
  if (G.has_epsillon[nt] && !ff->first_has_epsillon[nt]) {
    ff->first_has_epsillon[nt] = true;
    ff->follow_rule[nt] = G.rule_count[nt];
  }

  
  firstComputed[nt] = true;
}

void followAdd(FirstFollow *ff, NON_TERMINAL nt, TOKEN_TYPE t) {
  
  for (int i = 0; i < ff->follow_count[nt]; i++) {
    if (ff->follow[nt][i] == t) {
      return;
    }
  }
  ff->follow[nt][(ff->follow_count[nt])++] = t;
  return;
}

void followDepAdd(NON_TERMINAL nt, NON_TERMINAL a, NON_TERMINAL **followDep,
                  int *depCount) {
  for (int i = 0; i < depCount[nt]; i++) {
    if (followDep[nt][i] == a)
      return;
  }
  followDep[nt][(depCount[nt])++] = a;
  return;
}

void followHelper(FirstFollow *ff, grammar_rule rule, NON_TERMINAL LHS,
                  NON_TERMINAL **followDep, int *depCount) {

  for (int i = 0; i < rule.element_count - 1; i++) {

    if (rule.elements[i].terminal) {
      continue;
    }

    NON_TERMINAL nt = rule.elements[i].var.nt;

    int j;
    
    
    
    for (j = i + 1; j < rule.element_count; j++) {
      if (rule.elements[j].terminal) {
        followAdd(ff, nt, rule.elements[j].var.t);
        break;
      }

      NON_TERMINAL curr = rule.elements[j].var.nt;
      for (int k = 0; k < ff->first_count[curr]; k++) {
        followAdd(ff, nt, ff->first[curr][k]);
      }
      if (!ff->first_has_epsillon[curr]) {
        break;
      }
    }

    if (j == rule.element_count) {
      followDepAdd(nt, LHS, followDep, depCount);
    }
  }

  
  
  if (!(rule.elements[rule.element_count - 1].terminal)) {
    followDepAdd(rule.elements[rule.element_count - 1].var.nt, LHS, followDep,
                 depCount);
  }

  return;
}

void clearDependency(NON_TERMINAL nt, NON_TERMINAL **followDep, int *depCount,
                     FirstFollow *ff) {
  if (depCount[nt] == 0)
    return;
  int i = 0;
  while (depCount[nt] > 0) {
    NON_TERMINAL dep = followDep[nt][i];
    depCount[nt]--;
    i++;
    clearDependency(dep, followDep, depCount, ff);

    for (int j = 0; j < ff->follow_count[dep]; j++) {
      followAdd(ff, nt, ff->follow[dep][j]);
    }
  }
}

char *getNonTerminal(NON_TERMINAL nt) {
  switch (nt) {
  case NT_PROGRAM:
    return "NT_PROGRAM";
  case NT_MAINFUNCTION:
    return "NT_MAINFUNCTION";
  case NT_OTHERFUNCTIONS:
    return "NT_OTHERFUNCTIONS";
  case NT_FUNCTION:
    return "NT_FUNCTION";
  case NT_INPUT_PAR:
    return "NT_INPUT_PAR";
  case NT_OUTPUT_PAR:
    return "NT_OUTPUT_PAR";
  case NT_PARAMETER_LIST:
    return "NT_PARAMETER_LIST";
  case NT_DATATYPE:
    return "NT_DATATYPE";
  case NT_PRIMITIVEDATATYPE:
    return "NT_PRIMITIVEDATATYPE";
  case NT_CONSTRUCTEDDATATYPE:
    return "NT_CONSTRUCTEDDATATYPE";
  case NT_REMAINING_LIST:
    return "NT_REMAINING_LIST";
  case NT_STMTS:
    return "NT_STMTS";
  case NT_TYPEDEFINITIONS:
    return "NT_TYPEDEFINITIONS";
  case NT_ACTUALORREDEFINED:
    return "NT_ACTUALORREDEFINED";
  case NT_TYPEDEFINITION:
    return "NT_TYPEDEFINITION";
  case NT_FIELDDEFINITIONS:
    return "NT_FIELDDEFINITIONS";
  case NT_FIELDDEFINITION:
    return "NT_FIELDDEFINITION";
  case NT_FIELDTYPE:
    return "NT_FIELDTYPE";
  case NT_MOREFIELDS:
    return "NT_MOREFIELDS";
  case NT_DECLATRATIONS:
    return "NT_DECLATRATIONS";
  case NT_DECLATRATION:
    return "NT_DECLATRATION";
  case NT_GLOBAL_OR_NOT:
    return "NT_GLOBAL_OR_NOT";
  case NT_OTHERSTMTS:
    return "NT_OTHERSTMTS";
  case NT_STMT:
    return "NT_STMT";
  case NT_ASSIGNMENTSTMT:
    return "NT_ASSIGNMENTSTMT";
  case NT_SINGLEORRECID:
    return "NT_SINGLEORRECID";
  case NT_OPTION_SINGLE_CONSTRUCTED:
    return "NT_OPTION_SINGLE_CONSTRUCTED";
  case NT_ONEEXPANSION:
    return "NT_ONEEXPANSION";
  case NT_MOREEXPANSIONS:
    return "NT_MOREEXPANSIONS";
  case NT_FUNCALLSTMT:
    return "NT_FUNCALLSTMT";
  case NT_OUTPUTPARAMETERS:
    return "NT_OUTPUTPARAMETERS";
  case NT_INPUTPARAMETERS:
    return "NT_INPUTPARAMETERS";
  case NT_ITERATIVESTMT:
    return "NT_ITERATIVESTMT";
  case NT_CONDITIONALSTMT:
    return "NT_CONDITIONALSTMT";
  case NT_ELSEPART:
    return "NT_ELSEPART";
  case NT_IOSTMT:
    return "NT_IOSTMT";
  case NT_ARITHMETICEXPRESSION:
    return "NT_ARITHMETICEXPRESSION";
  case NT_EXPPRIME:
    return "NT_EXPPRIME";
  case NT_TERM:
    return "NT_TERM";
  case NT_TERMPRIME:
    return "NT_TERMPRIME";
  case NT_FACTOR:
    return "NT_FACTOR";
  case NT_HIGHPRECEDENCEOPERATORS:
    return "NT_HIGHPRECEDENCEOPERATORS";
  case NT_LOWPRECEDENCEOPERATORS:
    return "NT_LOWPRECEDENCEOPERATORS";
  case NT_BOOLEANEXPRESSION:
    return "NT_BOOLEANEXPRESSION";
  case NT_VAR:
    return "NT_VAR";
  case NT_LOGICALOP:
    return "NT_LOGICALOP";
  case NT_RELATIONALOP:
    return "NT_RELATIONALOP";
  case NT_RETURNSTMT:
    return "NT_RETURNSTMT";
  case NT_OPTIONALRETURN:
    return "NT_OPTIONALRETURN";
  case NT_IDLIST:
    return "NT_IDLIST";
  case NT_MORE_IDS:
    return "NT_MORE_IDS";
  case NT_DEFINETYPESTMT:
    return "NT_DEFINETYPESTMT";
  case NT_A:
    return "NT_A";
  default:
    return "INVALID_NON_TERMINAL";
  }
}

grammar initializeGrammar() {
  grammar G;

  
  G.rules[NT_PROGRAM][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_OTHERFUNCTIONS}},
                   (grammar_element){false, {.nt = NT_MAINFUNCTION}}},
      .element_count = 2};
  G.rule_count[NT_PROGRAM] = 1;
  G.has_epsillon[NT_PROGRAM] = false;

  
  G.rules[NT_MAINFUNCTION][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_MAIN}},
                                  (grammar_element){false, {.nt = NT_STMTS}},
                                  (grammar_element){true, {.t = TK_END}}},
                     .element_count = 3};
  G.rule_count[NT_MAINFUNCTION] = 1;
  G.has_epsillon[NT_MAINFUNCTION] = false;

  
  G.rules[NT_OTHERFUNCTIONS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_FUNCTION}},
                   (grammar_element){false, {.nt = NT_OTHERFUNCTIONS}}},
      .element_count = 2};
  G.rules[NT_OTHERFUNCTIONS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_OTHERFUNCTIONS] = 1;
  G.has_epsillon[NT_OTHERFUNCTIONS] = true; 

  
  G.rules[NT_FUNCTION][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_FUNID}},
                   (grammar_element){false, {.nt = NT_INPUT_PAR}},
                   (grammar_element){false, {.nt = NT_OUTPUT_PAR}},
                   (grammar_element){true, {.t = TK_SEM}},
                   (grammar_element){false, {.nt = NT_STMTS}},
                   (grammar_element){true, {.t = TK_END}}},
      .element_count = 6};
  G.rule_count[NT_FUNCTION] = 1;
  G.has_epsillon[NT_FUNCTION] = false;

  
  
  G.rules[NT_INPUT_PAR][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_INPUT}},
                   (grammar_element){true, {.t = TK_PARAMETER}},
                   (grammar_element){true, {.t = TK_LIST}},
                   (grammar_element){true, {.t = TK_SQL}},
                   (grammar_element){false, {.nt = NT_PARAMETER_LIST}},
                   (grammar_element){true, {.t = TK_SQR}}},
      .element_count = 6};
  G.rule_count[NT_INPUT_PAR] = 1;
  G.has_epsillon[NT_INPUT_PAR] = false;

  
  
  G.rules[NT_OUTPUT_PAR][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_OUTPUT}},
                   (grammar_element){true, {.t = TK_PARAMETER}},
                   (grammar_element){true, {.t = TK_LIST}},
                   (grammar_element){true, {.t = TK_SQL}},
                   (grammar_element){false, {.nt = NT_PARAMETER_LIST}},
                   (grammar_element){true, {.t = TK_SQR}}},
      .element_count = 6};
  G.rules[NT_OUTPUT_PAR][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_OUTPUT_PAR] = 1;
  G.has_epsillon[NT_OUTPUT_PAR] = true;

  
  G.rules[NT_PARAMETER_LIST][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_DATATYPE}},
                   (grammar_element){true, {.t = TK_ID}},
                   (grammar_element){false, {.nt = NT_REMAINING_LIST}}},
      .element_count = 3};
  G.rule_count[NT_PARAMETER_LIST] = 1;
  G.has_epsillon[NT_PARAMETER_LIST] = false;

  
  G.rules[NT_DATATYPE][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_PRIMITIVEDATATYPE}}},
      .element_count = 1};
  G.rules[NT_DATATYPE][1] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_CONSTRUCTEDDATATYPE}}},
      .element_count = 1};
  G.rule_count[NT_DATATYPE] = 2;
  G.has_epsillon[NT_DATATYPE] = false;

  
  G.rules[NT_PRIMITIVEDATATYPE][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_INT}}}, .element_count = 1};
  G.rules[NT_PRIMITIVEDATATYPE][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_REAL}}},
                     .element_count = 1};
  G.rule_count[NT_PRIMITIVEDATATYPE] = 2;
  G.has_epsillon[NT_PRIMITIVEDATATYPE] = false;

  
  G.rules[NT_CONSTRUCTEDDATATYPE][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_RECORD}},
                                  (grammar_element){true, {.t = TK_RUID}}},
                     .element_count = 2};
  G.rules[NT_CONSTRUCTEDDATATYPE][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_UNION}},
                                  (grammar_element){true, {.t = TK_RUID}}},
                     .element_count = 2};
  G.rules[NT_CONSTRUCTEDDATATYPE][2] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_RUID}}},
                     .element_count = 1};
  G.rule_count[NT_CONSTRUCTEDDATATYPE] = 3;
  G.has_epsillon[NT_CONSTRUCTEDDATATYPE] = false;

  
  G.rules[NT_REMAINING_LIST][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_COMMA}},
                   (grammar_element){false, {.nt = NT_PARAMETER_LIST}}},
      .element_count = 2};
  G.rules[NT_REMAINING_LIST][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_REMAINING_LIST] = 1;
  G.has_epsillon[NT_REMAINING_LIST] = true;

  
  G.rules[NT_STMTS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_TYPEDEFINITIONS}},
                   (grammar_element){false, {.nt = NT_DECLATRATIONS}},
                   (grammar_element){false, {.nt = NT_OTHERSTMTS}},
                   (grammar_element){false, {.nt = NT_RETURNSTMT}}},
      .element_count = 4};
  G.rule_count[NT_STMTS] = 1;
  G.has_epsillon[NT_STMTS] = false;

  
  G.rules[NT_TYPEDEFINITIONS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_ACTUALORREDEFINED}},
                   (grammar_element){false, {.nt = NT_TYPEDEFINITIONS}}},
      .element_count = 2};
  G.rules[NT_TYPEDEFINITIONS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_TYPEDEFINITIONS] = 1;
  G.has_epsillon[NT_TYPEDEFINITIONS] = true;

  
  G.rules[NT_ACTUALORREDEFINED][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_TYPEDEFINITION}}},
      .element_count = 1};
  G.rules[NT_ACTUALORREDEFINED][1] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_DEFINETYPESTMT}}},
      .element_count = 1};
  G.rule_count[NT_ACTUALORREDEFINED] = 2;
  G.has_epsillon[NT_ACTUALORREDEFINED] = false;

  
  
  G.rules[NT_TYPEDEFINITION][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_RECORD}},
                   (grammar_element){true, {.t = TK_RUID}},
                   (grammar_element){false, {.nt = NT_FIELDDEFINITIONS}},
                   (grammar_element){true, {.t = TK_ENDRECORD}}},
      .element_count = 4};
  G.rules[NT_TYPEDEFINITION][1] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_UNION}},
                   (grammar_element){true, {.t = TK_RUID}},
                   (grammar_element){false, {.nt = NT_FIELDDEFINITIONS}},
                   (grammar_element){true, {.t = TK_ENDUNION}}},
      .element_count = 4};
  G.rule_count[NT_TYPEDEFINITION] = 2;
  G.has_epsillon[NT_TYPEDEFINITION] = false;

  
  G.rules[NT_FIELDDEFINITIONS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_FIELDDEFINITION}},
                   (grammar_element){false, {.nt = NT_FIELDDEFINITION}},
                   (grammar_element){false, {.nt = NT_MOREFIELDS}}},
      .element_count = 3};
  G.rule_count[NT_FIELDDEFINITIONS] = 1;
  G.has_epsillon[NT_FIELDDEFINITIONS] = false;

  
  G.rules[NT_FIELDDEFINITION][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_TYPE}},
                   (grammar_element){false, {.nt = NT_FIELDTYPE}},
                   (grammar_element){true, {.t = TK_COLON}},
                   (grammar_element){true, {.t = TK_FIELDID}},
                   (grammar_element){true, {.t = TK_SEM}}},
      .element_count = 5};
  G.rule_count[NT_FIELDDEFINITION] = 1;
  G.has_epsillon[NT_FIELDDEFINITION] = false;

  
  G.rules[NT_FIELDTYPE][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_PRIMITIVEDATATYPE}}},
      .element_count = 1};
  G.rules[NT_FIELDTYPE][1] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_CONSTRUCTEDDATATYPE}}},
      .element_count = 1};
  G.rule_count[NT_FIELDTYPE] = 2;
  G.has_epsillon[NT_FIELDTYPE] = false;

  
  G.rules[NT_MOREFIELDS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_FIELDDEFINITION}},
                   (grammar_element){false, {.nt = NT_MOREFIELDS}}},
      .element_count = 2};
  G.rules[NT_MOREFIELDS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_MOREFIELDS] = 1;
  G.has_epsillon[NT_MOREFIELDS] = true;

  
  G.rules[NT_DECLATRATIONS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_DECLATRATION}},
                   (grammar_element){false, {.nt = NT_DECLATRATIONS}}},
      .element_count = 2};
  G.rules[NT_DECLATRATIONS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_DECLATRATIONS] = 1;
  G.has_epsillon[NT_DECLATRATIONS] = true;

  
  
  G.rules[NT_DECLATRATION][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_TYPE}},
                   (grammar_element){false, {.nt = NT_DATATYPE}},
                   (grammar_element){true, {.t = TK_COLON}},
                   (grammar_element){true, {.t = TK_ID}},
                   (grammar_element){false, {.nt = NT_GLOBAL_OR_NOT}},
                   (grammar_element){true, {.t = TK_SEM}}},
      .element_count = 6};
  G.rule_count[NT_DECLATRATION] = 1;
  G.has_epsillon[NT_DECLATRATION] = false;

  
  G.rules[NT_GLOBAL_OR_NOT][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_COLON}},
                                  (grammar_element){true, {.t = TK_GLOBAL}}},
                     .element_count = 2};
  G.rules[NT_GLOBAL_OR_NOT][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_GLOBAL_OR_NOT] = 1;
  G.has_epsillon[NT_GLOBAL_OR_NOT] = true;

  
  G.rules[NT_OTHERSTMTS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_STMT}},
                   (grammar_element){false, {.nt = NT_OTHERSTMTS}}},
      .element_count = 2};
  G.rules[NT_OTHERSTMTS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_OTHERSTMTS] = 1;
  G.has_epsillon[NT_OTHERSTMTS] = true;

  
  
  G.rules[NT_STMT][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_ASSIGNMENTSTMT}}},
      .element_count = 1};
  G.rules[NT_STMT][1] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_ITERATIVESTMT}}},
      .element_count = 1};
  G.rules[NT_STMT][2] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_CONDITIONALSTMT}}},
      .element_count = 1};
  G.rules[NT_STMT][3] =
      (grammar_rule){.elements = {(grammar_element){false, {.nt = NT_IOSTMT}}},
                     .element_count = 1};
  G.rules[NT_STMT][4] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_FUNCALLSTMT}}},
      .element_count = 1};
  G.rule_count[NT_STMT] = 5;
  G.has_epsillon[NT_STMT] = false;

  
  
  G.rules[NT_ASSIGNMENTSTMT][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_SINGLEORRECID}},
                   (grammar_element){true, {.t = TK_ASSIGNOP}},
                   (grammar_element){false, {.nt = NT_ARITHMETICEXPRESSION}},
                   (grammar_element){true, {.t = TK_SEM}}},
      .element_count = 4};
  G.rule_count[NT_ASSIGNMENTSTMT] = 1;
  G.has_epsillon[NT_ASSIGNMENTSTMT] = false;

  
  G.rules[NT_SINGLEORRECID][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_ID}},
                   (grammar_element){false,
                                     {.nt = NT_OPTION_SINGLE_CONSTRUCTED}}},
      .element_count = 2};
  G.rule_count[NT_SINGLEORRECID] = 1;
  G.has_epsillon[NT_SINGLEORRECID] = false;

  
  G.rules[NT_OPTION_SINGLE_CONSTRUCTED][0] = (grammar_rule){
      .elements =
          {
              (grammar_element){false, {.nt = NT_ONEEXPANSION}},
              (grammar_element){false, {.nt = NT_MOREEXPANSIONS}},
          },
      .element_count = 2};
  G.rules[NT_OPTION_SINGLE_CONSTRUCTED][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_OPTION_SINGLE_CONSTRUCTED] = 1;
  G.has_epsillon[NT_OPTION_SINGLE_CONSTRUCTED] = true;

  
  G.rules[NT_ONEEXPANSION][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_DOT}},
                                  (grammar_element){true, {.t = TK_FIELDID}}},
                     .element_count = 2};
  G.rule_count[NT_ONEEXPANSION] = 1;
  G.has_epsillon[NT_ONEEXPANSION] = false;

  
  G.rules[NT_MOREEXPANSIONS][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_ONEEXPANSION}},
                   (grammar_element){false, {.nt = NT_MOREEXPANSIONS}}},
      .element_count = 2};
  G.rules[NT_MOREEXPANSIONS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_MOREEXPANSIONS] = 1;
  G.has_epsillon[NT_MOREEXPANSIONS] = true;

  
  
  G.rules[NT_FUNCALLSTMT][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_OUTPUTPARAMETERS}},
                   (grammar_element){true, {.t = TK_CALL}},
                   (grammar_element){true, {.t = TK_FUNID}},
                   (grammar_element){true, {.t = TK_WITH}},
                   (grammar_element){true, {.t = TK_PARAMETERS}},
                   (grammar_element){false, {.nt = NT_INPUTPARAMETERS}},
                   (grammar_element){true, {.t = TK_SEM}}},
      .element_count = 7};
  G.rule_count[NT_FUNCALLSTMT] = 1;
  G.has_epsillon[NT_FUNCALLSTMT] = false;

  
  G.rules[NT_OUTPUTPARAMETERS][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_SQL}},
                                  (grammar_element){false, {.nt = NT_IDLIST}},
                                  (grammar_element){true, {.t = TK_SQR}},
                                  (grammar_element){true, {.t = TK_ASSIGNOP}}},
                     .element_count = 4};
  G.rules[NT_OUTPUTPARAMETERS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_OUTPUTPARAMETERS] = 1;
  G.has_epsillon[NT_OUTPUTPARAMETERS] = true;

  
  G.rules[NT_INPUTPARAMETERS][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_SQL}},
                                  (grammar_element){false, {.nt = NT_IDLIST}},
                                  (grammar_element){true, {.t = TK_SQR}}},
                     .element_count = 3};
  G.rule_count[NT_INPUTPARAMETERS] = 1;
  G.has_epsillon[NT_INPUTPARAMETERS] = false;

  
  
  G.rules[NT_ITERATIVESTMT][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_WHILE}},
                   (grammar_element){true, {.t = TK_OP}},
                   (grammar_element){false, {.nt = NT_BOOLEANEXPRESSION}},
                   (grammar_element){true, {.t = TK_CL}},
                   (grammar_element){false, {.nt = NT_STMT}},
                   (grammar_element){false, {.nt = NT_OTHERSTMTS}},
                   (grammar_element){true, {.t = TK_ENDWHILE}}},
      .element_count = 7};
  G.rule_count[NT_ITERATIVESTMT] = 1;
  G.has_epsillon[NT_ITERATIVESTMT] = false;

  
  
  G.rules[NT_CONDITIONALSTMT][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_IF}},
                   (grammar_element){true, {.t = TK_OP}},
                   (grammar_element){false, {.nt = NT_BOOLEANEXPRESSION}},
                   (grammar_element){true, {.t = TK_CL}},
                   (grammar_element){true, {.t = TK_THEN}},
                   (grammar_element){false, {.nt = NT_STMT}},
                   (grammar_element){false, {.nt = NT_OTHERSTMTS}},
                   (grammar_element){false, {.nt = NT_ELSEPART}}},
      .element_count = 8};
  G.rule_count[NT_CONDITIONALSTMT] = 1;
  G.has_epsillon[NT_CONDITIONALSTMT] = false;

  
  G.rules[NT_ELSEPART][0] =
      (grammar_rule){.elements =
                         {
                             (grammar_element){true, {.t = TK_ELSE}},
                             (grammar_element){false, {.nt = NT_STMT}},
                             (grammar_element){false, {.nt = NT_OTHERSTMTS}},
                             (grammar_element){true, {.t = TK_ENDIF}},
                         },
                     .element_count = 4};
  G.rules[NT_ELSEPART][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_ENDIF}}},
                     .element_count = 1};
  G.rule_count[NT_ELSEPART] = 2;
  G.has_epsillon[NT_ELSEPART] = false;

  
  
  G.rules[NT_IOSTMT][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_READ}},
                                  (grammar_element){true, {.t = TK_OP}},
                                  (grammar_element){false, {.nt = NT_VAR}},
                                  (grammar_element){true, {.t = TK_CL}},
                                  (grammar_element){true, {.t = TK_SEM}}},
                     .element_count = 5};
  G.rules[NT_IOSTMT][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_WRITE}},
                                  (grammar_element){true, {.t = TK_OP}},
                                  (grammar_element){false, {.nt = NT_VAR}},
                                  (grammar_element){true, {.t = TK_CL}},
                                  (grammar_element){true, {.t = TK_SEM}}},
                     .element_count = 5};
  G.rule_count[NT_IOSTMT] = 2;
  G.has_epsillon[NT_IOSTMT] = false;

  
  G.rules[NT_ARITHMETICEXPRESSION][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_TERM}},
                   (grammar_element){false, {.nt = NT_EXPPRIME}}},
      .element_count = 2};
  G.rule_count[NT_ARITHMETICEXPRESSION] = 1;
  G.has_epsillon[NT_ARITHMETICEXPRESSION] = false;

  
  G.rules[NT_EXPPRIME][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_LOWPRECEDENCEOPERATORS}},
                   (grammar_element){false, {.nt = NT_TERM}},
                   (grammar_element){false, {.nt = NT_EXPPRIME}}},
      .element_count = 3};
  G.rules[NT_EXPPRIME][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_EXPPRIME] = 1;
  G.has_epsillon[NT_EXPPRIME] = true;

  
  G.rules[NT_TERM][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_FACTOR}},
                   (grammar_element){false, {.nt = NT_TERMPRIME}}},
      .element_count = 2};
  G.rule_count[NT_TERM] = 1;
  G.has_epsillon[NT_TERM] = false;

  
  G.rules[NT_TERMPRIME][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_HIGHPRECEDENCEOPERATORS}},
                   (grammar_element){false, {.nt = NT_FACTOR}},
                   (grammar_element){false, {.nt = NT_TERMPRIME}}},
      .element_count = 3};
  G.rules[NT_TERMPRIME][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_TERMPRIME] = 1;
  G.has_epsillon[NT_TERMPRIME] = true;

  
  G.rules[NT_FACTOR][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_OP}},
                   (grammar_element){false, {.nt = NT_ARITHMETICEXPRESSION}},
                   (grammar_element){true, {.t = TK_CL}}},
      .element_count = 3};
  G.rules[NT_FACTOR][1] =
      (grammar_rule){.elements = {(grammar_element){false, {.nt = NT_VAR}}},
                     .element_count = 1};
  G.rule_count[NT_FACTOR] = 2;
  G.has_epsillon[NT_FACTOR] = false;

  
  G.rules[NT_HIGHPRECEDENCEOPERATORS][0] =
      (grammar_rule){.elements =
                         {
                             (grammar_element){true, {.t = TK_MUL}},
                         },
                     .element_count = 1};
  G.rules[NT_HIGHPRECEDENCEOPERATORS][1] =
      (grammar_rule){.elements =
                         {
                             (grammar_element){true, {.t = TK_DIV}},
                         },
                     .element_count = 1};
  G.rule_count[NT_HIGHPRECEDENCEOPERATORS] = 2;
  G.has_epsillon[NT_HIGHPRECEDENCEOPERATORS] = false;

  
  G.rules[NT_LOWPRECEDENCEOPERATORS][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_PLUS}}},
                     .element_count = 1};
  G.rules[NT_LOWPRECEDENCEOPERATORS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_MINUS}}},
                     .element_count = 1};
  G.rule_count[NT_LOWPRECEDENCEOPERATORS] = 2;
  G.has_epsillon[NT_LOWPRECEDENCEOPERATORS] = false;

  
  
  
  
  G.rules[NT_BOOLEANEXPRESSION][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_OP}},
                   (grammar_element){false, {.nt = NT_BOOLEANEXPRESSION}},
                   (grammar_element){true, {.t = TK_CL}},
                   (grammar_element){false, {.nt = NT_LOGICALOP}},
                   (grammar_element){true, {.t = TK_OP}},
                   (grammar_element){false, {.nt = NT_BOOLEANEXPRESSION}},
                   (grammar_element){true, {.t = TK_CL}}},
      .element_count = 7};
  G.rules[NT_BOOLEANEXPRESSION][1] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_VAR}},
                   (grammar_element){false, {.nt = NT_RELATIONALOP}},
                   (grammar_element){false, {.nt = NT_VAR}}},
      .element_count = 3};
  G.rules[NT_BOOLEANEXPRESSION][2] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_NOT}},
                   (grammar_element){true, {.t = TK_OP}},
                   (grammar_element){false, {.nt = NT_BOOLEANEXPRESSION}},
                   (grammar_element){true, {.t = TK_CL}}},
      .element_count = 4};
  G.rule_count[NT_BOOLEANEXPRESSION] = 3;
  G.has_epsillon[NT_BOOLEANEXPRESSION] = false;

  
  G.rules[NT_VAR][0] = (grammar_rule){
      .elements = {(grammar_element){false, {.nt = NT_SINGLEORRECID}}},
      .element_count = 1};
  G.rules[NT_VAR][1] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_NUM}}},
      .element_count = 1,
  };
  G.rules[NT_VAR][2] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_RNUM}}},
      .element_count = 1,
  };
  G.rule_count[NT_VAR] = 3;
  G.has_epsillon[NT_VAR] = false;

  
  G.rules[NT_LOGICALOP][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_AND}}}, .element_count = 1};
  G.rules[NT_LOGICALOP][1] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_OR}}}, .element_count = 1};
  G.rule_count[NT_LOGICALOP] = 2;
  G.has_epsillon[NT_LOGICALOP] = false;

  
  G.rules[NT_RELATIONALOP][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_LT}}}, .element_count = 1};
  G.rules[NT_RELATIONALOP][1] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_LE}}}, .element_count = 1};
  G.rules[NT_RELATIONALOP][2] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_EQ}}}, .element_count = 1};
  G.rules[NT_RELATIONALOP][3] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_GT}}}, .element_count = 1};
  G.rules[NT_RELATIONALOP][4] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_GE}}}, .element_count = 1};
  G.rules[NT_RELATIONALOP][5] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_NE}}}, .element_count = 1};

  G.rule_count[NT_RELATIONALOP] = 6;
  G.has_epsillon[NT_RELATIONALOP] = false;

  
  G.rules[NT_RETURNSTMT][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_RETURN}},
                   (grammar_element){false, {.nt = NT_OPTIONALRETURN}},
                   (grammar_element){true, {.t = TK_SEM}}},
      .element_count = 3};
  G.rule_count[NT_RETURNSTMT] = 1;
  G.has_epsillon[NT_RETURNSTMT] = false;

  
  G.rules[NT_OPTIONALRETURN][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_SQL}},
                                  (grammar_element){false, {.nt = NT_IDLIST}},
                                  (grammar_element){true, {.t = TK_SQR}}},
                     .element_count = 3};
  G.rules[NT_OPTIONALRETURN][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_OPTIONALRETURN] = 1;
  G.has_epsillon[NT_OPTIONALRETURN] = true;

  
  G.rules[NT_IDLIST][0] = (grammar_rule){
      .elements = {(grammar_element){true, {.t = TK_ID}},
                   (grammar_element){false, {.nt = NT_MORE_IDS}}},
      .element_count = 2};
  G.rule_count[NT_IDLIST] = 1;
  G.has_epsillon[NT_IDLIST] = false;

  
  G.rules[NT_MORE_IDS][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_COMMA}},
                                  (grammar_element){false, {.nt = NT_IDLIST}}},
                     .element_count = 2};
  G.rules[NT_MORE_IDS][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = EPSILLON}}},
                     .element_count = 1};
  G.rule_count[NT_MORE_IDS] = 1;
  G.has_epsillon[NT_MORE_IDS] = true;

  
  G.rules[NT_DEFINETYPESTMT][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_DEFINETYPE}},
                                  (grammar_element){false, {.nt = NT_A}},
                                  (grammar_element){true, {.t = TK_RUID}},
                                  (grammar_element){true, {.t = TK_AS}},
                                  (grammar_element){true, {.t = TK_RUID}}},
                     .element_count = 5};
  G.rule_count[NT_DEFINETYPESTMT] = 1;
  G.has_epsillon[NT_DEFINETYPESTMT] = false;

  
  G.rules[NT_A][0] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_RECORD}}},
                     .element_count = 1};
  G.rules[NT_A][1] =
      (grammar_rule){.elements = {(grammar_element){true, {.t = TK_UNION}}},
                     .element_count = 1};
  G.rule_count[NT_A] = 2;
  G.has_epsillon[NT_A] = false;
  return G;
}

/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "token_model.h"

#define NON_TERMINAL_COUNT 53
#define MAX_RULE_SIZE 15

typedef enum {
    NT_PROGRAM,
    NT_MAINFUNCTION,
    NT_OTHERFUNCTIONS,
    NT_FUNCTION,
    NT_INPUT_PAR,
    NT_OUTPUT_PAR,
    NT_PARAMETER_LIST,
    NT_DATATYPE,
    NT_PRIMITIVEDATATYPE,
    NT_CONSTRUCTEDDATATYPE,
    NT_REMAINING_LIST,
    NT_STMTS,
    NT_TYPEDEFINITIONS,
    NT_ACTUALORREDEFINED,
    NT_TYPEDEFINITION,
    NT_FIELDDEFINITIONS,
    NT_FIELDDEFINITION,
    NT_FIELDTYPE,
    NT_MOREFIELDS,
    NT_DECLATRATIONS,
    NT_DECLATRATION,
    NT_GLOBAL_OR_NOT,
    NT_OTHERSTMTS,
    NT_STMT,
    NT_ASSIGNMENTSTMT,
    NT_SINGLEORRECID,
    NT_OPTION_SINGLE_CONSTRUCTED,
    NT_ONEEXPANSION,
    NT_MOREEXPANSIONS,
    NT_FUNCALLSTMT,
    NT_OUTPUTPARAMETERS,
    NT_INPUTPARAMETERS,
    NT_ITERATIVESTMT,
    NT_CONDITIONALSTMT,
    NT_ELSEPART,
    NT_IOSTMT,
    NT_ARITHMETICEXPRESSION,
    NT_EXPPRIME,
    NT_TERM,
    NT_TERMPRIME,
    NT_FACTOR,
    NT_HIGHPRECEDENCEOPERATORS,
    NT_LOWPRECEDENCEOPERATORS,
    NT_BOOLEANEXPRESSION,
    NT_VAR,
    NT_LOGICALOP,
    NT_RELATIONALOP,
    NT_RETURNSTMT,
    NT_OPTIONALRETURN,
    NT_IDLIST,
    NT_MORE_IDS,
    NT_DEFINETYPESTMT,
    NT_A,
} NON_TERMINAL;

typedef struct {
    bool terminal;
    union {
        TOKEN_TYPE t;
        NON_TERMINAL nt;
    } var;
} grammar_element;

typedef struct {
    grammar_element elements[MAX_RULE_SIZE];
    int element_count;
} grammar_rule;

typedef struct {
    grammar_rule rules[NON_TERMINAL_COUNT][MAX_RULE_SIZE];
    int rule_count[NON_TERMINAL_COUNT];
    bool has_epsillon[NON_TERMINAL_COUNT];
} grammar;

typedef struct {
    TOKEN_TYPE first[NON_TERMINAL_COUNT][MAX_RULE_SIZE];
    int first_count[NON_TERMINAL_COUNT];
    bool first_has_epsillon[NON_TERMINAL_COUNT];
    int rule_no[NON_TERMINAL_COUNT][MAX_RULE_SIZE];

    TOKEN_TYPE follow[NON_TERMINAL_COUNT][MAX_RULE_SIZE];
    int follow_count[NON_TERMINAL_COUNT];
    int follow_rule[NON_TERMINAL_COUNT];
} FirstAndFollow;

typedef struct {
    int table[NON_TERMINAL_COUNT][TOTAL_TOKENS];
} table;

typedef struct {
    grammar_element symbol;
    int line;
    char *lexeme;
    int lexemeSize;
} parseTreeElement;

typedef struct parseTree parseTree;

struct parseTree {
    parseTreeElement ele;
    struct parseTree *parent;
    int no_of_children;
    struct parseTree *children[MAX_RULE_SIZE];
};

#endif

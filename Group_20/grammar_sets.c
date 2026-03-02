/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#include "grammar_sets.h"
#include "grammar_model.h"
#include "token_model.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    const char *dsl_name;
    NON_TERMINAL nt;
} NonTerminalMap;

typedef struct {
    const char *name;
    TOKEN_TYPE token;
} TokenMap;

static const NonTerminalMap k_non_terminal_map[] = {
    {"program", NT_PROGRAM},
    {"mainFunction", NT_MAINFUNCTION},
    {"otherFunctions", NT_OTHERFUNCTIONS},
    {"function", NT_FUNCTION},
    {"input_par", NT_INPUT_PAR},
    {"output_par", NT_OUTPUT_PAR},
    {"parameter_list", NT_PARAMETER_LIST},
    {"dataType", NT_DATATYPE},
    {"primitiveDatatype", NT_PRIMITIVEDATATYPE},
    {"constructedDatatype", NT_CONSTRUCTEDDATATYPE},
    {"remaining_list", NT_REMAINING_LIST},
    {"stmts", NT_STMTS},
    {"typeDefinitions", NT_TYPEDEFINITIONS},
    {"actualOrRedefined", NT_ACTUALORREDEFINED},
    {"typeDefinition", NT_TYPEDEFINITION},
    {"fieldDefinitions", NT_FIELDDEFINITIONS},
    {"fieldDefinition", NT_FIELDDEFINITION},
    {"fieldType", NT_FIELDTYPE},
    {"moreFields", NT_MOREFIELDS},
    {"declarations", NT_DECLATRATIONS},
    {"declaration", NT_DECLATRATION},
    {"global_or_not", NT_GLOBAL_OR_NOT},
    {"otherStmts", NT_OTHERSTMTS},
    {"stmt", NT_STMT},
    {"assignmentStmt", NT_ASSIGNMENTSTMT},
    {"singleOrRecId", NT_SINGLEORRECID},
    {"option_single_constructed", NT_OPTION_SINGLE_CONSTRUCTED},
    {"oneExpansion", NT_ONEEXPANSION},
    {"moreExpansions", NT_MOREEXPANSIONS},
    {"funCallStmt", NT_FUNCALLSTMT},
    {"outputParameters", NT_OUTPUTPARAMETERS},
    {"inputParameters", NT_INPUTPARAMETERS},
    {"iterativeStmt", NT_ITERATIVESTMT},
    {"conditionalStmt", NT_CONDITIONALSTMT},
    {"elsePart", NT_ELSEPART},
    {"ioStmt", NT_IOSTMT},
    {"arithmeticExpression", NT_ARITHMETICEXPRESSION},
    {"expPrime", NT_EXPPRIME},
    {"term", NT_TERM},
    {"termPrime", NT_TERMPRIME},
    {"factor", NT_FACTOR},
    {"highPrecedenceOperators", NT_HIGHPRECEDENCEOPERATORS},
    {"lowPrecedenceOperators", NT_LOWPRECEDENCEOPERATORS},
    {"booleanExpression", NT_BOOLEANEXPRESSION},
    {"var", NT_VAR},
    {"logicalOp", NT_LOGICALOP},
    {"relationalOp", NT_RELATIONALOP},
    {"returnStmt", NT_RETURNSTMT},
    {"optionalReturn", NT_OPTIONALRETURN},
    {"idList", NT_IDLIST},
    {"more_ids", NT_MORE_IDS},
    {"definetypestmt", NT_DEFINETYPESTMT},
    {"A", NT_A},
};

static const TokenMap k_token_map[] = {
    {"TK_ASSIGNOP", TK_ASSIGNOP},
    {"TK_COMMENT", TK_COMMENT},
    {"TK_FIELDID", TK_FIELDID},
    {"TK_ID", TK_ID},
    {"TK_NUM", TK_NUM},
    {"TK_RNUM", TK_RNUM},
    {"TK_FUNID", TK_FUNID},
    {"TK_RUID", TK_RUID},
    {"TK_WITH", TK_WITH},
    {"TK_PARAMETERS", TK_PARAMETERS},
    {"TK_END", TK_END},
    {"TK_WHILE", TK_WHILE},
    {"TK_UNION", TK_UNION},
    {"TK_ENDUNION", TK_ENDUNION},
    {"TK_DEFINETYPE", TK_DEFINETYPE},
    {"TK_AS", TK_AS},
    {"TK_TYPE", TK_TYPE},
    {"TK_MAIN", TK_MAIN},
    {"TK_GLOBAL", TK_GLOBAL},
    {"TK_PARAMETER", TK_PARAMETER},
    {"TK_LIST", TK_LIST},
    {"TK_SQL", TK_SQL},
    {"TK_SQR", TK_SQR},
    {"TK_INPUT", TK_INPUT},
    {"TK_OUTPUT", TK_OUTPUT},
    {"TK_INT", TK_INT},
    {"TK_REAL", TK_REAL},
    {"TK_COMMA", TK_COMMA},
    {"TK_SEM", TK_SEM},
    {"TK_COLON", TK_COLON},
    {"TK_DOT", TK_DOT},
    {"TK_ENDWHILE", TK_ENDWHILE},
    {"TK_OP", TK_OP},
    {"TK_CL", TK_CL},
    {"TK_IF", TK_IF},
    {"TK_THEN", TK_THEN},
    {"TK_ENDIF", TK_ENDIF},
    {"TK_READ", TK_READ},
    {"TK_WRITE", TK_WRITE},
    {"TK_RETURN", TK_RETURN},
    {"TK_PLUS", TK_PLUS},
    {"TK_MINUS", TK_MINUS},
    {"TK_MUL", TK_MUL},
    {"TK_DIV", TK_DIV},
    {"TK_CALL", TK_CALL},
    {"TK_RECORD", TK_RECORD},
    {"TK_ENDRECORD", TK_ENDRECORD},
    {"TK_ELSE", TK_ELSE},
    {"TK_AND", TK_AND},
    {"TK_OR", TK_OR},
    {"TK_NOT", TK_NOT},
    {"TK_LT", TK_LT},
    {"TK_LE", TK_LE},
    {"TK_EQ", TK_EQ},
    {"TK_GT", TK_GT},
    {"TK_GE", TK_GE},
    {"TK_NE", TK_NE},
};

static const char *k_non_terminal_names[NON_TERMINAL_COUNT] = {
    "NT_PROGRAM",
    "NT_MAINFUNCTION",
    "NT_OTHERFUNCTIONS",
    "NT_FUNCTION",
    "NT_INPUT_PAR",
    "NT_OUTPUT_PAR",
    "NT_PARAMETER_LIST",
    "NT_DATATYPE",
    "NT_PRIMITIVEDATATYPE",
    "NT_CONSTRUCTEDDATATYPE",
    "NT_REMAINING_LIST",
    "NT_STMTS",
    "NT_TYPEDEFINITIONS",
    "NT_ACTUALORREDEFINED",
    "NT_TYPEDEFINITION",
    "NT_FIELDDEFINITIONS",
    "NT_FIELDDEFINITION",
    "NT_FIELDTYPE",
    "NT_MOREFIELDS",
    "NT_DECLATRATIONS",
    "NT_DECLATRATION",
    "NT_GLOBAL_OR_NOT",
    "NT_OTHERSTMTS",
    "NT_STMT",
    "NT_ASSIGNMENTSTMT",
    "NT_SINGLEORRECID",
    "NT_OPTION_SINGLE_CONSTRUCTED",
    "NT_ONEEXPANSION",
    "NT_MOREEXPANSIONS",
    "NT_FUNCALLSTMT",
    "NT_OUTPUTPARAMETERS",
    "NT_INPUTPARAMETERS",
    "NT_ITERATIVESTMT",
    "NT_CONDITIONALSTMT",
    "NT_ELSEPART",
    "NT_IOSTMT",
    "NT_ARITHMETICEXPRESSION",
    "NT_EXPPRIME",
    "NT_TERM",
    "NT_TERMPRIME",
    "NT_FACTOR",
    "NT_HIGHPRECEDENCEOPERATORS",
    "NT_LOWPRECEDENCEOPERATORS",
    "NT_BOOLEANEXPRESSION",
    "NT_VAR",
    "NT_LOGICALOP",
    "NT_RELATIONALOP",
    "NT_RETURNSTMT",
    "NT_OPTIONALRETURN",
    "NT_IDLIST",
    "NT_MORE_IDS",
    "NT_DEFINETYPESTMT",
    "NT_A",
};

static const char *k_productions[] = {
    "<program> -> <otherFunctions> <mainFunction>",
    "<mainFunction> -> TK_MAIN <stmts> TK_END",
    "<otherFunctions> -> <function> <otherFunctions> | eps",
    "<function> -> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END",
    "<input_par> -> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR",
    "<output_par> -> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR | eps",
    "<parameter_list> -> <dataType> TK_ID <remaining_list>",
    "<dataType> -> <primitiveDatatype> | <constructedDatatype>",
    "<primitiveDatatype> -> TK_INT | TK_REAL",
    "<constructedDatatype> -> TK_RECORD TK_RUID | TK_UNION TK_RUID | TK_RUID",
    "<remaining_list> -> TK_COMMA <parameter_list> | eps",
    "<stmts> -> <typeDefinitions> <declarations> <otherStmts> <returnStmt>",
    "<typeDefinitions> -> <actualOrRedefined> <typeDefinitions> | eps",
    "<actualOrRedefined> -> <typeDefinition> | <definetypestmt>",
    "<typeDefinition> -> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD",
    "<typeDefinition> -> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION",
    "<fieldDefinitions> -> <fieldDefinition> <fieldDefinition> <moreFields>",
    "<fieldDefinition> -> TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM",
    "<fieldType> -> <primitiveDatatype> | <constructedDatatype>",
    "<moreFields> -> <fieldDefinition> <moreFields> | eps",
    "<declarations> -> <declaration> <declarations> | eps",
    "<declaration> -> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM",
    "<global_or_not> -> TK_COLON TK_GLOBAL | eps",
    "<otherStmts> -> <stmt> <otherStmts> | eps",
    "<stmt> -> <assignmentStmt> | <iterativeStmt> | <conditionalStmt> | <ioStmt> | <funCallStmt>",
    "<assignmentStmt> -> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM",
    "<singleOrRecId> -> TK_ID <option_single_constructed>",
    "<option_single_constructed> -> <oneExpansion> <moreExpansions> | eps",
    "<oneExpansion> -> TK_DOT TK_FIELDID",
    "<moreExpansions> -> <oneExpansion> <moreExpansions> | eps",
    "<funCallStmt> -> <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM",
    "<outputParameters> -> TK_SQL <idList> TK_SQR TK_ASSIGNOP | eps",
    "<inputParameters> -> TK_SQL <idList> TK_SQR",
    "<iterativeStmt> -> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE",
    "<conditionalStmt> -> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts> <elsePart>",
    "<elsePart> -> TK_ELSE <stmt> <otherStmts> TK_ENDIF | TK_ENDIF",
    "<ioStmt> -> TK_READ TK_OP <var> TK_CL TK_SEM | TK_WRITE TK_OP <var> TK_CL TK_SEM",
    "<arithmeticExpression> -> <term> <expPrime>",
    "<expPrime> -> <lowPrecedenceOperators> <term> <expPrime> | eps",
    "<term> -> <factor> <termPrime>",
    "<termPrime> -> <highPrecedenceOperators> <factor> <termPrime> | eps",
    "<factor> -> TK_OP <arithmeticExpression> TK_CL | <var>",
    "<highPrecedenceOperators> -> TK_MUL | TK_DIV",
    "<lowPrecedenceOperators> -> TK_PLUS | TK_MINUS",
    "<booleanExpression> -> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL",
    "<booleanExpression> -> <var> <relationalOp> <var>",
    "<booleanExpression> -> TK_NOT TK_OP <booleanExpression> TK_CL",
    "<var> -> <singleOrRecId> | TK_NUM | TK_RNUM",
    "<logicalOp> -> TK_AND | TK_OR",
    "<relationalOp> -> TK_LT | TK_LE | TK_EQ | TK_GT | TK_GE | TK_NE",
    "<returnStmt> -> TK_RETURN <optionalReturn> TK_SEM",
    "<optionalReturn> -> TK_SQL <idList> TK_SQR | eps",
    "<idList> -> TK_ID <more_ids>",
    "<more_ids> -> TK_COMMA <idList> | eps",
    "<definetypestmt> -> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID",
    "<A> -> TK_RECORD | TK_UNION",
};

static char *trim_spaces(char *text)
{
    while (*text != '\0' && isspace((unsigned char)*text))
    {
        ++text;
    }

    if (*text == '\0')
    {
        return text;
    }

    char *end = text + strlen(text) - 1;
    while (end >= text && isspace((unsigned char)*end))
    {
        *end = '\0';
        --end;
    }

    return text;
}

static bool lookup_non_terminal(const char *name, NON_TERMINAL *nt)
{
    for (size_t index = 0; index < sizeof(k_non_terminal_map) / sizeof(k_non_terminal_map[0]); ++index)
    {
        if (strcmp(k_non_terminal_map[index].dsl_name, name) == 0)
        {
            *nt = k_non_terminal_map[index].nt;
            return true;
        }
    }
    return false;
}

static bool lookup_token_type(const char *name, TOKEN_TYPE *token)
{
    for (size_t index = 0; index < sizeof(k_token_map) / sizeof(k_token_map[0]); ++index)
    {
        if (strcmp(k_token_map[index].name, name) == 0)
        {
            *token = k_token_map[index].token;
            return true;
        }
    }
    return false;
}

static bool decode_angle_non_terminal(const char *symbol, NON_TERMINAL *nt)
{
    size_t length = strlen(symbol);
    if (length < 3 || symbol[0] != '<' || symbol[length - 1] != '>')
    {
        return false;
    }

    char content[64];
    size_t content_len = length - 2;
    if (content_len >= sizeof(content))
    {
        return false;
    }

    memcpy(content, symbol + 1, content_len);
    content[content_len] = '\0';
    return lookup_non_terminal(content, nt);
}

static bool parse_alternative(const char *alt_text, grammar_rule *rule, bool *is_epsilon)
{
    char local[512];
    strncpy(local, alt_text, sizeof(local) - 1);
    local[sizeof(local) - 1] = '\0';

    char *segment = trim_spaces(local);
    if (strcmp(segment, "eps") == 0)
    {
        *is_epsilon = true;
        rule->element_count = 0;
        return true;
    }

    *is_epsilon = false;
    rule->element_count = 0;

    for (char *word = strtok(segment, " \t\r\n"); word != NULL; word = strtok(NULL, " \t\r\n"))
    {
        if (rule->element_count >= MAX_RULE_SIZE)
        {
            return false;
        }

        grammar_element *slot = &rule->elements[rule->element_count++];
        NON_TERMINAL parsed_nt;
        TOKEN_TYPE parsed_token;

        if (decode_angle_non_terminal(word, &parsed_nt))
        {
            slot->terminal = false;
            slot->var.nt = parsed_nt;
            continue;
        }

        if (!lookup_token_type(word, &parsed_token))
        {
            return false;
        }

        slot->terminal = true;
        slot->var.t = parsed_token;
    }

    return rule->element_count > 0;
}

static void append_epsilon_rule(grammar *G, NON_TERMINAL nt)
{
    int eps_index = G->rule_count[nt];
    G->rules[nt][eps_index].element_count = 1;
    G->rules[nt][eps_index].elements[0].terminal = true;
    G->rules[nt][eps_index].elements[0].var.t = EPSILLON;
}

void followDepAdd(NON_TERMINAL nt, NON_TERMINAL dep, NON_TERMINAL **followDep, int *depCount)
{
    for (int idx = 0; idx < depCount[nt]; ++idx)
    {
        if (followDep[nt][idx] == dep)
        {
            return;
        }
    }
    followDep[nt][depCount[nt]++] = dep;
}

void followAdd(FirstAndFollow *ff, NON_TERMINAL nt, TOKEN_TYPE t)
{
    for (int idx = 0; idx < ff->follow_count[nt]; ++idx)
    {
        if (ff->follow[nt][idx] == t)
        {
            return;
        }
    }
    ff->follow[nt][ff->follow_count[nt]++] = t;
}

void clearDependency(NON_TERMINAL nt, NON_TERMINAL **followDep, int *depCount, FirstAndFollow *ff)
{
    while (depCount[nt] > 0)
    {
        NON_TERMINAL dep = followDep[nt][--depCount[nt]];
        if (depCount[dep] > 0)
        {
            clearDependency(dep, followDep, depCount, ff);
        }

        for (int index = 0; index < ff->follow_count[dep]; ++index)
        {
            followAdd(ff, nt, ff->follow[dep][index]);
        }
    }
}

void followHelper(FirstAndFollow *ff, grammar_rule rule, NON_TERMINAL lhs, NON_TERMINAL **followDep, int *depCount)
{
    for (int left = 0; left < rule.element_count; ++left)
    {
        if (rule.elements[left].terminal)
        {
            continue;
        }

        NON_TERMINAL target = rule.elements[left].var.nt;
        bool needs_lhs_follow = true;

        for (int right = left + 1; right < rule.element_count; ++right)
        {
            grammar_element symbol = rule.elements[right];
            if (symbol.terminal)
            {
                followAdd(ff, target, symbol.var.t);
                needs_lhs_follow = false;
                break;
            }

            NON_TERMINAL nt = symbol.var.nt;
            for (int first_idx = 0; first_idx < ff->first_count[nt]; ++first_idx)
            {
                followAdd(ff, target, ff->first[nt][first_idx]);
            }

            if (!ff->first_has_epsillon[nt])
            {
                needs_lhs_follow = false;
                break;
            }
        }

        if (needs_lhs_follow)
        {
            followDepAdd(target, lhs, followDep, depCount);
        }
    }
}

void firstUnion(FirstAndFollow *ff, NON_TERMINAL dst, NON_TERMINAL src)
{
    for (int src_idx = 0; src_idx < ff->first_count[src]; ++src_idx)
    {
        TOKEN_TYPE token = ff->first[src][src_idx];
        bool already_present = false;

        for (int dst_idx = 0; dst_idx < ff->first_count[dst]; ++dst_idx)
        {
            if (ff->first[dst][dst_idx] == token)
            {
                already_present = true;
                break;
            }
        }

        if (!already_present)
        {
            ff->first[dst][ff->first_count[dst]++] = token;
        }
    }
}

void computeFirstRec(FirstAndFollow *ff, NON_TERMINAL nt, grammar G, bool *fr)
{
    if (fr[nt])
    {
        return;
    }

    for (int rule_idx = 0; rule_idx < G.rule_count[nt]; ++rule_idx)
    {
        grammar_rule rule = G.rules[nt][rule_idx];
        int before_merge = ff->first_count[nt];
        bool nullable_prefix = true;

        for (int symbol_idx = 0; symbol_idx < rule.element_count; ++symbol_idx)
        {
            grammar_element symbol = rule.elements[symbol_idx];

            if (symbol.terminal)
            {
                bool duplicate = false;
                for (int idx = 0; idx < ff->first_count[nt]; ++idx)
                {
                    if (ff->first[nt][idx] == symbol.var.t)
                    {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate)
                {
                    ff->first[nt][ff->first_count[nt]++] = symbol.var.t;
                }
                nullable_prefix = false;
                break;
            }

            NON_TERMINAL child = symbol.var.nt;
            computeFirstRec(ff, child, G, fr);
            firstUnion(ff, nt, child);

            if (!ff->first_has_epsillon[child])
            {
                nullable_prefix = false;
                break;
            }
        }

        if (nullable_prefix)
        {
            ff->follow_rule[nt] = rule_idx;
        }

        for (int idx = before_merge; idx < ff->first_count[nt]; ++idx)
        {
            ff->rule_no[nt][idx] = rule_idx;
        }
    }

    if (G.has_epsillon[nt])
    {
        ff->first_has_epsillon[nt] = true;
        ff->follow_rule[nt] = G.rule_count[nt];
    }

    fr[nt] = true;
}

char *getNonTerminal(NON_TERMINAL nt)
{
    if (nt < 0 || nt >= NON_TERMINAL_COUNT)
    {
        return "INVALID_NON_TERMINAL";
    }
    return (char *)k_non_terminal_names[nt];
}

grammar initializeGrammar()
{
    grammar G;
    memset(&G, 0, sizeof(G));

    bool has_epsilon[NON_TERMINAL_COUNT];
    memset(has_epsilon, 0, sizeof(has_epsilon));

    for (size_t prod_idx = 0; prod_idx < sizeof(k_productions) / sizeof(k_productions[0]); ++prod_idx)
    {
        char line[512];
        strncpy(line, k_productions[prod_idx], sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';

        char *arrow = strstr(line, "->");
        if (arrow == NULL)
        {
            continue;
        }

        *arrow = '\0';
        char *lhs_text = trim_spaces(line);
        char *rhs_text = trim_spaces(arrow + 2);

        NON_TERMINAL lhs;
        if (!decode_angle_non_terminal(lhs_text, &lhs))
        {
            continue;
        }

        char *cursor = rhs_text;
        while (cursor != NULL)
        {
            char *separator = strchr(cursor, '|');
            if (separator != NULL)
            {
                *separator = '\0';
            }

            grammar_rule rule;
            bool is_eps = false;
            if (parse_alternative(cursor, &rule, &is_eps))
            {
                if (is_eps)
                {
                    has_epsilon[lhs] = true;
                }
                else if (G.rule_count[lhs] < MAX_RULE_SIZE)
                {
                    G.rules[lhs][G.rule_count[lhs]++] = rule;
                }
            }

            if (separator == NULL)
            {
                cursor = NULL;
            }
            else
            {
                cursor = separator + 1;
            }
        }
    }

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        G.has_epsillon[nt] = has_epsilon[nt];
        if (has_epsilon[nt] && G.rule_count[nt] < MAX_RULE_SIZE)
        {
            append_epsilon_rule(&G, (NON_TERMINAL)nt);
        }
    }

    return G;
}

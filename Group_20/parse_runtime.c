/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#include "parse_runtime.h"

#include "grammar_model.h"
#include "grammar_sets.h"
#include "lexer.h"
#include "token_model.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    parseTree *node;
    grammar_element symbol;
} ParseFrame;

typedef struct {
    parseTree *node;
    int stage;
    int next_child;
} InorderFrame;

typedef struct {
    parseTree *node;
} NodeFrame;

static parseTree *new_parse_node(parseTree *parent)
{
    parseTree *node = (parseTree *)malloc(sizeof(parseTree));
    if (node == NULL)
    {
        return NULL;
    }

    node->ele.symbol.terminal = false;
    node->ele.symbol.var.nt = NT_PROGRAM;
    node->ele.line = -1;
    node->ele.lexeme = NULL;
    node->ele.lexemeSize = 0;
    node->parent = parent;
    node->no_of_children = 0;

    for (int idx = 0; idx < MAX_RULE_SIZE; ++idx)
    {
        node->children[idx] = NULL;
    }

    return node;
}

static void print_line_entry(const parseTree *node, FILE *fp)
{
    const bool is_leaf = (node->no_of_children == 0);
    const bool is_terminal = node->ele.symbol.terminal;

    const char *lexeme_text = (is_leaf && node->ele.lexeme != NULL) ? node->ele.lexeme : "----";
    const char *token_name = is_terminal ? getTokenName(node->ele.symbol.var.t) : "----";

    const bool is_number_terminal =
        is_terminal &&
        (node->ele.symbol.var.t == TK_NUM || node->ele.symbol.var.t == TK_RNUM) &&
        node->ele.lexeme != NULL;
    const char *numeric_value = is_number_terminal ? node->ele.lexeme : "----";

    const char *parent_name = "ROOT";
    if (node->parent != NULL)
    {
        parent_name = getNonTerminal(node->parent->ele.symbol.var.nt);
    }

    const char *node_symbol =
        is_terminal ? getTokenName(node->ele.symbol.var.t) : getNonTerminal(node->ele.symbol.var.nt);

    fprintf(fp, "%-30s", lexeme_text);
    if (is_leaf && node->ele.line >= 0)
    {
        fprintf(fp, "%-30d", node->ele.line);
    }
    else
    {
        fprintf(fp, "%-30s", "----");
    }

    fprintf(fp,
            "%-30s%-30s%-30s%-30s%-30s\n",
            token_name,
            numeric_value,
            parent_name,
            is_leaf ? "yes" : "no",
            node_symbol);
}

void printParseTreeInorder(parseTree *PT, FILE *fp)
{
    if (PT == NULL || fp == NULL)
    {
        return;
    }

    fprintf(fp,
            "%-30s%-30s%-30s%-30s%-30s%-30s%-30s\n\n",
            "lexeme CurrentNode",
            "lineno",
            "tokenName",
            "valueIfNumber",
            "parentNodeSymbol",
            "isLeafNode(yes/no)",
            "NodeSymbol");

    InorderFrame frames[4096];
    int top = 0;
    frames[top].node = PT;
    frames[top].stage = 0;
    frames[top].next_child = 1;

    while (top >= 0)
    {
        InorderFrame *frame = &frames[top];
        parseTree *node = frame->node;

        if (node == NULL)
        {
            --top;
            continue;
        }

        if (frame->stage == 0)
        {
            frame->stage = 1;
            if (node->no_of_children > 0 && node->children[0] != NULL)
            {
                ++top;
                frames[top].node = node->children[0];
                frames[top].stage = 0;
                frames[top].next_child = 1;
                continue;
            }
        }

        if (frame->stage == 1)
        {
            print_line_entry(node, fp);
            frame->stage = 2;
            frame->next_child = 1;
        }

        if (frame->stage == 2)
        {
            int child_index = frame->next_child;
            while (child_index < node->no_of_children && node->children[child_index] == NULL)
            {
                ++child_index;
            }

            if (child_index < node->no_of_children)
            {
                frame->next_child = child_index + 1;
                ++top;
                frames[top].node = node->children[child_index];
                frames[top].stage = 0;
                frames[top].next_child = 1;
                continue;
            }
        }

        --top;
    }
}

static void emit_dot_text(FILE *fp, const char *text)
{
    if (fp == NULL || text == NULL)
    {
        return;
    }

    for (const char *cursor = text; *cursor != '\0'; ++cursor)
    {
        if (*cursor == '"' || *cursor == '\\')
        {
            fputc('\\', fp);
        }

        if (*cursor == '\n' || *cursor == '\r')
        {
            fputs("\\n", fp);
            continue;
        }

        fputc(*cursor, fp);
    }
}

void writeParseTreeDot(parseTree *PT, FILE *fp)
{
    if (PT == NULL || fp == NULL)
    {
        return;
    }

    fputs("digraph ParseTree {\n", fp);
    fputs("  rankdir=TB;\n", fp);
    fputs("  node [shape=box, style=rounded, fontsize=10];\n", fp);

    NodeFrame stack[4096];
    int top = 0;
    stack[top].node = PT;

    while (top >= 0)
    {
        parseTree *node = stack[top--].node;
        if (node == NULL)
        {
            continue;
        }

        const char *symbol = node->ele.symbol.terminal
                                 ? getTokenName(node->ele.symbol.var.t)
                                 : getNonTerminal(node->ele.symbol.var.nt);

        unsigned long long node_id = (unsigned long long)(uintptr_t)node;
        fprintf(fp, "  n%llx [label=\"", node_id);
        emit_dot_text(fp, symbol);

        if (node->ele.symbol.terminal && node->ele.lexeme != NULL)
        {
            fputs("\\nlexeme: ", fp);
            emit_dot_text(fp, node->ele.lexeme);
        }
        if (node->ele.line >= 0)
        {
            fprintf(fp, "\\nline: %d", node->ele.line);
        }
        fputs("\"];\n", fp);

        for (int idx = node->no_of_children - 1; idx >= 0; --idx)
        {
            parseTree *child = node->children[idx];
            if (child == NULL)
            {
                continue;
            }

            unsigned long long child_id = (unsigned long long)(uintptr_t)child;
            fprintf(fp, "  n%llx -> n%llx;\n", node_id, child_id);

            ++top;
            stack[top].node = child;
        }
    }

    fputs("}\n", fp);
}

void buildParseTable(FirstAndFollow ff, table *T)
{
    if (T == NULL)
    {
        return;
    }

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        for (int tk = 0; tk < TOTAL_TOKENS; ++tk)
        {
            T->table[nt][tk] = -1;
        }
    }

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        for (int idx = 0; idx < ff.first_count[nt]; ++idx)
        {
            TOKEN_TYPE lookahead = ff.first[nt][idx];
            T->table[nt][lookahead] = ff.rule_no[nt][idx];
        }

        if (ff.follow_rule[nt] != -1)
        {
            for (int idx = 0; idx < ff.follow_count[nt]; ++idx)
            {
                TOKEN_TYPE lookahead = ff.follow[nt][idx];
                T->table[nt][lookahead] = ff.follow_rule[nt];
            }
            continue;
        }

        for (int idx = 0; idx < ff.follow_count[nt]; ++idx)
        {
            TOKEN_TYPE lookahead = ff.follow[nt][idx];
            if (T->table[nt][lookahead] == -1)
            {
                T->table[nt][lookahead] = -2;
            }
        }
    }
}

FirstAndFollow computeFirstandFollowSets(grammar G)
{
    FirstAndFollow ff;
    memset(&ff, 0, sizeof(ff));

    bool first_ready[NON_TERMINAL_COUNT];
    int dep_count[NON_TERMINAL_COUNT];
    NON_TERMINAL **dependencies =
        (NON_TERMINAL **)malloc(sizeof(NON_TERMINAL *) * NON_TERMINAL_COUNT);

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        ff.first_count[nt] = 0;
        ff.follow_count[nt] = 0;
        ff.first_has_epsillon[nt] = false;
        ff.follow_rule[nt] = -1;

        first_ready[nt] = false;
        dep_count[nt] = 0;

        dependencies[nt] = (NON_TERMINAL *)calloc(MAX_RULE_SIZE, sizeof(NON_TERMINAL));
    }

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        computeFirstRec(&ff, (NON_TERMINAL)nt, G, first_ready);
    }

    ff.follow[NT_PROGRAM][0] = DOLLAR;
    ff.follow_count[NT_PROGRAM] = 1;

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        for (int rule = 0; rule < G.rule_count[nt]; ++rule)
        {
            followHelper(&ff, G.rules[nt][rule], (NON_TERMINAL)nt, dependencies, dep_count);
        }
    }

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        if (dep_count[nt] > 0)
        {
            clearDependency((NON_TERMINAL)nt, dependencies, dep_count, &ff);
        }
    }

    for (int nt = 0; nt < NON_TERMINAL_COUNT; ++nt)
    {
        free(dependencies[nt]);
    }
    free(dependencies);

    return ff;
}

static twinBuffer initialize_twin_buffer(FILE *fp)
{
    twinBuffer buffer = (twinBuffer)malloc(sizeof(TWIN_BUFFER));
    if (buffer == NULL)
    {
        return NULL;
    }

    for (int idx = 0; idx < 2 * BUFFER_SIZE; ++idx)
    {
        buffer->buffer[idx] = '\0';
    }

    buffer->index = 2 * BUFFER_SIZE - 1;
    buffer->line = 1;

    populate_buffer(buffer, fp);
    buffer->index = 0;
    populate_buffer(buffer, fp);

    return buffer;
}

static void push_frame(ParseFrame *stack, int *top, parseTree *node, grammar_element symbol)
{
    ++(*top);
    stack[*top].node = node;
    stack[*top].symbol = symbol;
}

static parseTree *append_child(parseTree *parent, int slot, grammar_element symbol)
{
    parseTree *child = new_parse_node(parent);
    if (child == NULL)
    {
        return NULL;
    }

    child->ele.symbol.terminal = symbol.terminal;
    if (symbol.terminal)
    {
        child->ele.symbol.var.t = symbol.var.t;
    }
    else
    {
        child->ele.symbol.var.nt = symbol.var.nt;
    }

    parent->children[slot] = child;
    return child;
}

static void expand_non_terminal(parseTree *node,
                                NON_TERMINAL nt,
                                int rule_no,
                                grammar G,
                                ParseFrame *stack,
                                int *top)
{
    if (G.has_epsillon[nt] && rule_no == G.rule_count[nt])
    {
        node->no_of_children = 1;

        grammar_element eps;
        eps.terminal = true;
        eps.var.t = EPSILLON;

        (void)append_child(node, 0, eps);
        return;
    }

    grammar_rule rule = G.rules[nt][rule_no];
    node->no_of_children = rule.element_count;

    parseTree *created[MAX_RULE_SIZE];
    for (int idx = 0; idx < rule.element_count; ++idx)
    {
        created[idx] = append_child(node, idx, rule.elements[idx]);
    }

    for (int idx = rule.element_count - 1; idx >= 0; --idx)
    {
        push_frame(stack, top, created[idx], rule.elements[idx]);
    }
}

static void report_terminal_mismatch(tokenInfo token, TOKEN_TYPE expected)
{
    printf("Line %02d: Syntax Error : The token %s for lexeme %s does not match with the expected token %s\n",
           token->line,
           getTokenName(token->type),
           token->lexeme,
           getTokenName(expected));
}

static void report_invalid_token(tokenInfo token, NON_TERMINAL nt)
{
    printf("Line %02d: Syntax Error : Invalid token %s encountered with value %s stack top %s\n",
           token->line,
           getTokenName(token->type),
           token->lexeme,
           getNonTerminal(nt));
}

parseTree *parseInputSourceCodeStream(table T, FirstAndFollow ff, grammar G, FILE *fp)
{
    (void)ff;

    twinBuffer B = initialize_twin_buffer(fp);
    initializeLookupTable();

    ParseFrame parse_stack[1024];
    int top = -1;

    parseTree *PT = new_parse_node(NULL);
    if (PT == NULL)
    {
        free(B);
        return NULL;
    }

    PT->ele.symbol.terminal = false;
    PT->ele.symbol.var.nt = NT_PROGRAM;

    grammar_element dollar;
    dollar.terminal = true;
    dollar.var.t = DOLLAR;

    grammar_element start;
    start.terminal = false;
    start.var.nt = NT_PROGRAM;

    push_frame(parse_stack, &top, NULL, dollar);
    push_frame(parse_stack, &top, PT, start);

    tokenInfo lookahead = nextToken(B, fp);

    bool err = false;
    int old_line = -1;

    while (B->buffer[B->index] != '\0' && top >= 0)
    {
        ParseFrame current = parse_stack[top];

        if (current.symbol.terminal)
        {
            if (current.symbol.var.t == DOLLAR && lookahead->type == DOLLAR)
            {
                break;
            }

            if (current.symbol.var.t == lookahead->type)
            {
                if (current.node != NULL)
                {
                    current.node->ele.symbol.terminal = true;
                    current.node->ele.symbol.var.t = lookahead->type;
                    current.node->ele.line = lookahead->line;
                    current.node->ele.lexeme = lookahead->lexeme;
                    current.node->ele.lexemeSize = lookahead->lexemeSize;
                }

                --top;
                free(lookahead);
                lookahead = nextToken(B, fp);
                continue;
            }

            err = true;
            if (old_line == lookahead->line)
            {
                free(lookahead);
                lookahead = nextToken(B, fp);
                continue;
            }

            old_line = lookahead->line;
            report_terminal_mismatch(lookahead, current.symbol.var.t);
            --top;
            continue;
        }

        NON_TERMINAL nt = current.symbol.var.nt;
        int rule_no = T.table[nt][lookahead->type];

        if (rule_no == -1)
        {
            err = true;
            if (old_line == lookahead->line)
            {
                free(lookahead);
                lookahead = nextToken(B, fp);
                continue;
            }

            old_line = lookahead->line;
            report_invalid_token(lookahead, nt);
            free(lookahead);
            lookahead = nextToken(B, fp);
            continue;
        }

        if (rule_no == -2)
        {
            err = true;
            if (old_line == lookahead->line)
            {
                free(lookahead);
                lookahead = nextToken(B, fp);
                continue;
            }

            old_line = lookahead->line;
            report_invalid_token(lookahead, nt);
            --top;
            continue;
        }

        --top;
        expand_non_terminal(current.node, nt, rule_no, G, parse_stack, &top);
    }

    if (!(top == 0 && parse_stack[top].symbol.terminal && parse_stack[top].symbol.var.t == DOLLAR))
    {
        err = true;
        printf("Syntax Error : Tokens parsed but stack not empty\n");
    }
    else if (lookahead->type != DOLLAR)
    {
        err = true;
        printf("Syntax Error : Stack empty but tokens not parsed\n");
    }

    free(B);

    if (!err)
    {
        printf("Input source code is syntactically correct...........\n");
    }
    else
    {
        printf("COMPILATION FAILED\n");
    }

    return PT;
}


#include "lexer/lexer.h"
#include "core/token_model.h"
#include "core/grammar_model.h"
#include "parser/first_follow.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void buildParseTable(FirstFollow F, table *T) {
  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    for (int j = 0; j < TOTAL_TOKENS; j++) {
      T->table[i][j] = -1;
    }
  }
  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    for (int j = 0; j < F.first_count[i]; j++) {
      T->table[i][F.first[i][j]] = F.rule_no[i][j];
    }
    if (F.follow_rule[i] != -1) {
      for (int j = 0; j < F.follow_count[i]; j++) {
        T->table[i][F.follow[i][j]] = F.follow_rule[i];
      }
    } else {
      for (int j = 0; j < F.follow_count[i]; j++) {
        if (T->table[i][F.follow[i][j]] == -1) {
          T->table[i][F.follow[i][j]] = -2;
        }
      }
    }
  }
}

FirstFollow computeFirstFollowSet(grammar G) {
  FirstFollow ff;

  bool firstComputed[NON_TERMINAL_COUNT];
  NON_TERMINAL **followDep;
  followDep =
      (NON_TERMINAL **)malloc(sizeof(NON_TERMINAL *) * NON_TERMINAL_COUNT);

  int depCount[NON_TERMINAL_COUNT];

  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    ff.first_count[i] = 0;
    ff.follow_count[i] = 0;
    ff.first_has_epsillon[i] = false;
    ff.follow_rule[i] = -1;
    firstComputed[i] = false;
    depCount[i] = 0;
    followDep[i] = (NON_TERMINAL *)calloc(MAX_RULE_SIZE, sizeof(NON_TERMINAL));
  }

  
  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    computeFirstRec(&ff, (NON_TERMINAL)i, G, firstComputed);
  }

  
  ff.follow[NT_PROGRAM][0] = DOLLAR;
  ff.follow_count[NT_PROGRAM] = 1;

  
  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    for (int j = 0; j < G.rule_count[i]; j++) {
      followHelper(&ff, G.rules[i][j], (NON_TERMINAL)i, followDep, depCount);
    }
  }

  
  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    if (depCount[i] > 0) {
      clearDependency(i, followDep, depCount, &ff);
    }
  }

  for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
    free(followDep[i]);
  }
  free(followDep);

  return ff;
}

parseTree *parseInputSourceCodeStream(table T, FirstFollow F, grammar G,
                                      FILE *fp) {
  (void)F;
  
  grammar_element *symbolStack[200];
  parseTree *treeNodeStack[200];

  
  twinBuffer B = (twinBuffer)malloc(sizeof(TWIN_BUFFER));
  for (int i = 0; i < 2 * BUFFER_SIZE; i++) {
    B->buffer[i] = '\0';
  }
  B->index = 2 * BUFFER_SIZE - 1;
  B->line = 1;
  populate_buffer(B, fp);
  B->index = 0;
  populate_buffer(B, fp);

  
  initializeLookupTable();

  int symbolStackTop = 0;
  int treeNodeStackTop = 0;
  int lookAheadPointer = 0;
  int oldLineNo = -1;
  bool error_encountered = false;

  parseTree *root = malloc(sizeof(parseTree));
  root->ele.symbol.terminal = false;
  root->ele.symbol.var.nt = NT_PROGRAM;
  root->ele.line = -1;
  root->parent = NULL;

  treeNodeStack[treeNodeStackTop] = root;

  
  grammar_element *dollar = (grammar_element *)malloc(sizeof(grammar_element));
  dollar->terminal = true;
  dollar->var.t = DOLLAR;
  grammar_element *program = (grammar_element *)malloc(sizeof(grammar_element));
  program->terminal = false;
  program->var.nt = NT_PROGRAM;

  
  symbolStack[symbolStackTop] = dollar;

  
  symbolStackTop++;
  symbolStack[symbolStackTop] = program;

  
  tokenInfo a = nextToken(B, fp);
  while (B->buffer[B->index] != '\0' && symbolStackTop >= 0) {
    grammar_element *X = symbolStack[symbolStackTop];

    
    if (X->terminal) {
      if (X->var.t == DOLLAR && a->type == DOLLAR) {
        
        
        break;
      } else if (X->var.t == a->type) {
        
        
        parseTree *node = treeNodeStack[treeNodeStackTop];
        node->ele.symbol.terminal = true;
        node->ele.symbol.var.t = a->type;
        node->ele.line = a->line;
        node->ele.lexeme = a->lexeme;
        node->ele.lexemeSize = a->lexemeSize;

        free(X);
        symbolStack[symbolStackTop] = NULL;
        symbolStackTop--;
        treeNodeStackTop--;
        lookAheadPointer++;
        free(a);
        a = nextToken(B, fp);
      } else {
        
        error_encountered = true;
        if (oldLineNo == a->line) {
          lookAheadPointer++;
          free(a);
          a = nextToken(B, fp);
          continue;
        } else {
          oldLineNo = a->line;
          printf(
              "Line %02d: Syntax Error : The token %s for lexeme %s does not "
              "match with the expected token %s\n",
              a->line, getTokenName(a->type), a->lexeme,
              getTokenName(X->var.t));
          free(X);
          symbolStack[symbolStackTop] = NULL;
          symbolStackTop--;
          treeNodeStackTop--;
        }
      }
    } else {
      
      NON_TERMINAL nt = X->var.nt;
      int rule_no = T.table[nt][a->type];
      if (rule_no == -1) {
        
        error_encountered = true;
        if (oldLineNo == a->line) {
          lookAheadPointer++;
          free(a);
          a = nextToken(B, fp);
          continue;
        } else {
          oldLineNo = a->line;
          printf("Line %02d: Syntax Error : Invalid token %s encountered with "
                 "value %s stack top %s\n",
                 a->line, getTokenName(a->type), a->lexeme, getNonTerminal(nt));
          lookAheadPointer++;
          free(a);
          a = nextToken(B, fp);
        }
      } else if (rule_no == -2) {
        
        error_encountered = true;
        if (oldLineNo == a->line) {
          lookAheadPointer++;
          free(a);
          a = nextToken(B, fp);
          continue;
        } else {
          oldLineNo = a->line;
          printf("Line %02d: Syntax Error : Invalid token %s encountered with "
                 "value %s stack top %s\n",
                 a->line, getTokenName(a->type), a->lexeme, getNonTerminal(nt));
          free(X);
          symbolStack[symbolStackTop] = NULL;
          symbolStackTop--;
          treeNodeStackTop--;
        }
      } else {
        
        
        grammar_rule rule = G.rules[nt][rule_no];
        parseTree *node = treeNodeStack[treeNodeStackTop];
        treeNodeStackTop--;
        free(X);
        symbolStack[symbolStackTop] = NULL;
        symbolStackTop--;

        if (G.has_epsillon[nt] && rule_no == G.rule_count[nt]) {
          
          
          node->no_of_children = 1;
          parseTree *childNode = (parseTree *)malloc(sizeof(parseTree));
          childNode->ele.symbol.terminal = true;
          childNode->ele.symbol.var.t = EPSILLON;
          childNode->ele.line = -1;
          childNode->parent = node;
          childNode->no_of_children = 0;
          node->children[0] = childNode;
        } else {
          
          

          node->no_of_children = rule.element_count;

          
          for (int i = rule.element_count - 1; i >= 0; i--) {
            parseTree *childNode = (parseTree *)malloc(sizeof(parseTree));

            childNode->ele.symbol.terminal = rule.elements[i].terminal;
            childNode->ele.line = -1;

            if (rule.elements[i].terminal) {
              childNode->ele.symbol.var.t = rule.elements[i].var.t;
            } else {
              childNode->ele.symbol.var.nt = rule.elements[i].var.nt;
            }

            childNode->parent = node;
            childNode->no_of_children = 0;
            node->children[i] = childNode;

            treeNodeStack[++treeNodeStackTop] = childNode;
            grammar_element *ele =
                (grammar_element *)malloc(sizeof(grammar_element));
            ele->terminal = rule.elements[i].terminal;
            if (ele->terminal) {
              ele->var.t = rule.elements[i].var.t;
            } else {
              ele->var.nt = rule.elements[i].var.nt;
            }

            symbolStack[++symbolStackTop] = ele;
          }
        }
      }
    }
  }
  
  
  if (!(symbolStackTop == 0 && symbolStack[symbolStackTop]->var.t == DOLLAR)) {
    error_encountered = true;
    printf("Syntax Error : Tokens parsed but stack not empty\n");
  } else if (a->type != DOLLAR) {
    error_encountered = true;
    printf("Syntax Error : Stack empty but tokens not parsed\n");
  }
  
  free(B);

  if (!error_encountered)
    printf("Input source code is syntactically correct...........\n");
  else
    printf("COMPILATION FAILED\n");

  
  while (symbolStackTop >= 0) {
    if (symbolStack[symbolStackTop] != NULL) {
      free(symbolStack[symbolStackTop]);
    }
    symbolStackTop--;
  }

  return root;
}

static void printLineEntry(parseTree *PT, FILE *outfile) {
  const int isLeaf = (PT->no_of_children == 0);
  const int isTerminal = PT->ele.symbol.terminal;
  const char *lexeme = (isLeaf && PT->ele.lexeme != NULL) ? PT->ele.lexeme : "----";
  const char *lineText = (isLeaf && PT->ele.line >= 0) ? NULL : "----";
  const char *tokenName = isTerminal ? getTokenName(PT->ele.symbol.var.t) : "----";
  const char *valueIfNumber =
      (isTerminal && (PT->ele.symbol.var.t == TK_NUM || PT->ele.symbol.var.t == TK_RNUM) &&
       PT->ele.lexeme != NULL)
          ? PT->ele.lexeme
          : "----";

  const char *parentSymbol = "ROOT";
  if (PT->parent != NULL) {
    parentSymbol = getNonTerminal(PT->parent->ele.symbol.var.nt);
  }

  const char *nodeSymbol =
      isTerminal ? getTokenName(PT->ele.symbol.var.t) : getNonTerminal(PT->ele.symbol.var.nt);

  fprintf(outfile, "%-30s", lexeme);
  if (lineText == NULL) {
    fprintf(outfile, "%-30d", PT->ele.line);
  } else {
    fprintf(outfile, "%-30s", lineText);
  }
  fprintf(outfile, "%-30s%-30s%-30s%-30s%-30s\n", tokenName, valueIfNumber,
          parentSymbol, isLeaf ? "yes" : "no", nodeSymbol);
}

static void printParseTreeInorderRec(parseTree *PT, FILE *outfile) {
  if (PT == NULL) {
    return;
  }
  if (PT->no_of_children != 0 && PT->children[0] != NULL) {
    printParseTreeInorderRec(PT->children[0], outfile);
  }
  if (outfile != NULL) {
    printLineEntry(PT, outfile);
    for (int i = 1; i < PT->no_of_children; i++) {
      if (PT->children[i] != NULL) {
        printParseTreeInorderRec(PT->children[i], outfile);
      }
    }
  }
}

void printParseTreeInorder(parseTree *PT, FILE *outfile) {
  if (PT == NULL || outfile == NULL) {
    return;
  }

  fprintf(outfile, "%-30s%-30s%-30s%-30s%-30s%-30s%-30s\n\n",
          "lexeme CurrentNode", "lineno", "tokenName", "valueIfNumber",
          "parentNodeSymbol", "isLeafNode(yes/no)", "NodeSymbol");
  printParseTreeInorderRec(PT, outfile);
}

static void writeEscapedDotText(FILE *outfile, const char *text) {
  if (text == NULL) {
    return;
  }

  for (const char *p = text; *p != '\0'; p++) {
    if (*p == '"' || *p == '\\') {
      fputc('\\', outfile);
    }
    if (*p == '\n' || *p == '\r') {
      fputs("\\n", outfile);
      continue;
    }
    fputc(*p, outfile);
  }
}

static void writeDotNodeRecursive(parseTree *node, FILE *outfile) {
  if (node == NULL) {
    return;
  }

  const char *symbol = NULL;
  if (node->ele.symbol.terminal) {
    symbol = getTokenName(node->ele.symbol.var.t);
  } else {
    symbol = getNonTerminal(node->ele.symbol.var.nt);
  }

  unsigned long long nodeId = (unsigned long long)(uintptr_t)node;
  fprintf(outfile, "  n%llx [label=\"", nodeId);
  writeEscapedDotText(outfile, symbol);

  if (node->ele.symbol.terminal && node->ele.lexeme != NULL) {
    fputs("\\nlexeme: ", outfile);
    writeEscapedDotText(outfile, node->ele.lexeme);
  }
  if (node->ele.line >= 0) {
    fprintf(outfile, "\\nline: %d", node->ele.line);
  }
  fprintf(outfile, "\"];\n");

  for (int i = 0; i < node->no_of_children; i++) {
    parseTree *child = node->children[i];
    if (child == NULL) {
      continue;
    }

    unsigned long long childId = (unsigned long long)(uintptr_t)child;
    fprintf(outfile, "  n%llx -> n%llx;\n", nodeId, childId);
    writeDotNodeRecursive(child, outfile);
  }
}

void writeParseTreeDot(parseTree *PT, FILE *outfile) {
  if (PT == NULL || outfile == NULL) {
    return;
  }

  fputs("digraph ParseTree {\n", outfile);
  fputs("  rankdir=TB;\n", outfile);
  fputs("  node [shape=box, style=rounded, fontsize=10];\n", outfile);
  writeDotNodeRecursive(PT, outfile);
  fputs("}\n", outfile);
}

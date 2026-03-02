/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#include "parse_runtime.h"
#include "lexer.h"
#include "token_model.h"
#include "grammar_model.h"
#include "grammar_sets.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



static void printLineEntry(parseTree *PT, FILE *fp)
   {
const int isLf = (PT->no_of_children == 0);
  const int isTerm = PT->ele.symbol.terminal;
const char *lexeme = (isLf && PT->ele.lexeme != NULL) ? PT->ele.lexeme : "----";
    const char *lineText = (isLf && PT->ele.line >= 0) ? NULL : "----";
const char *tok = isTerm ? getTokenName(PT->ele.symbol.var.t) : "----";
 const char *val =
 (isTerm &&
  (PT->ele.symbol.var.t == TK_NUM || PT->ele.symbol.var.t == TK_RNUM) &&
     PT->ele.lexeme != NULL)
? PT->ele.lexeme
    : "----";

const char *psym = "ROOT";
if (PT->parent != NULL)

{
   psym = getNonTerminal(PT->parent->ele.symbol.var.nt);
      }


   const char *nsym = isTerm ? getTokenName(PT->ele.symbol.var.t)
: getNonTerminal(PT->ele.symbol.var.nt);

fprintf(fp, "%-30s", lexeme);
  if (lineText == NULL)

{
  fprintf(fp, "%-30d", PT->ele.line);
}
else
{
 fprintf(fp, "%-30s", lineText);
}
  fprintf(fp, "%-30s%-30s%-30s%-30s%-30s\n", tok, val, psym, isLf ? "yes" : "no",
 nsym);
 }

static void printParseTreeInorderRec(parseTree *PT, FILE *fp)

{
     if (PT == NULL)

 {
   return;

  }
if (PT->no_of_children != 0 && PT->children[0] != NULL)


  {
   printParseTreeInorderRec(PT->children[0], fp);
 }
if (fp != NULL)
{
printLineEntry(PT, fp);
     for (int childIndex = 1; childIndex < PT->no_of_children; childIndex++)
   {
     if (PT->children[childIndex] != NULL)


  {
     printParseTreeInorderRec(PT->children[childIndex], fp);

 }

}
}
}

 void printParseTreeInorder(parseTree *PT, FILE *fp)

 {
  if (PT == NULL || fp == NULL)

   {
    return;

}

 fprintf(fp, "%-30s%-30s%-30s%-30s%-30s%-30s%-30s\n\n", "lexeme CurrentNode",
"lineno", "tokenName", "valueIfNumber", "parentNodeSymbol",
"isLeafNode(yes/no)", "NodeSymbol");
  printParseTreeInorderRec(PT, fp);

}


 static void writeEscapedDotText(FILE *fp, const char *text)

{
if (text == NULL)
    {
    return;
   }


for (const char *p = text; *p != '\0'; p++)
 {
   if (*p == '"' || *p == '\\')
{
   fputc('\\', fp);
 }
   if (*p == '\n' || *p == '\r')
 {
fputs("\\n", fp);
    continue;
}

fputc(*p, fp);

    }
   }



static void writeDotNodeRecursive(parseTree *node, FILE *fp)

  {
  if (node == NULL)
{
   return;
   }


   const char *symbol = NULL;
  if (node->ele.symbol.terminal)
{
   symbol = getTokenName(node->ele.symbol.var.t);

}

  else
{
symbol = getNonTerminal(node->ele.symbol.var.nt);

}



  unsigned long long nodeId = (unsigned long long)(uintptr_t)node;
   fprintf(fp, "  n%llx [label=\"", nodeId);
writeEscapedDotText(fp, symbol);

if (node->ele.symbol.terminal && node->ele.lexeme != NULL)
{
     fputs("\\nlexeme: ", fp);
  writeEscapedDotText(fp, node->ele.lexeme);
}
if (node->ele.line >= 0)

  {
    fprintf(fp, "\\nline: %d", node->ele.line);
}
fprintf(fp, "\"];\n");

  for (int i = 0; i < node->no_of_children; i++)

{
 parseTree *child = node->children[i];
 if (child == NULL)
   {
      continue;
   }

unsigned long long childId = (unsigned long long)(uintptr_t)child;
   fprintf(fp, "  n%llx -> n%llx;\n", nodeId, childId);
  writeDotNodeRecursive(child, fp);
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
  writeDotNodeRecursive(PT, fp);
     fputs("}\n", fp);
  }

   void buildParseTable(FirstAndFollow ff, table *T)

{
    for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)
{
      for (int tokenIndex = 0; tokenIndex < TOTAL_TOKENS; tokenIndex++)
   {
    T->table[ntIndex][tokenIndex] = -1;
     }

   }


  for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)
 {
 for (int firstIndex = 0; firstIndex < ff.first_count[ntIndex]; firstIndex++)
  {

  T->table[ntIndex][ff.first[ntIndex][firstIndex]] =
   ff.rule_no[ntIndex][firstIndex];

    }

if (ff.follow_rule[ntIndex] != -1)
  {
  for (int followIndex = 0; followIndex < ff.follow_count[ntIndex];
     followIndex++)
  {
 T->table[ntIndex][ff.follow[ntIndex][followIndex]] =
  ff.follow_rule[ntIndex];
 }
}

 else

   {
for (int followIndex = 0; followIndex < ff.follow_count[ntIndex];
followIndex++)
  {
    if (T->table[ntIndex][ff.follow[ntIndex][followIndex]] == -1)

       {
       T->table[ntIndex][ff.follow[ntIndex][followIndex]] = -2;
  }

}

  }


}

}

FirstAndFollow computeFirstandFollowSets(grammar G)

 {
FirstAndFollow ff;

    bool fr[NON_TERMINAL_COUNT];
     NON_TERMINAL **fdep =
     (NON_TERMINAL **)malloc(sizeof(NON_TERMINAL *) * NON_TERMINAL_COUNT);
    int depCount[NON_TERMINAL_COUNT];

for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)

   {
   ff.first_count[ntIndex] = 0;
       ff.follow_count[ntIndex] = 0;
  ff.first_has_epsillon[ntIndex] = false;
    ff.follow_rule[ntIndex] = -1;
  fr[ntIndex] = false;
    depCount[ntIndex] = 0;
    fdep[ntIndex] = (NON_TERMINAL *)calloc(MAX_RULE_SIZE, sizeof(NON_TERMINAL));
  }


for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)
 {
  computeFirstRec(&ff, (NON_TERMINAL)ntIndex, G, fr);

 }



ff.follow[NT_PROGRAM][0] = DOLLAR;
  ff.follow_count[NT_PROGRAM] = 1;


   for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)
{
   for (int ruleIndex = 0; ruleIndex < G.rule_count[ntIndex]; ruleIndex++)
     {
       followHelper(&ff, G.rules[ntIndex][ruleIndex], (NON_TERMINAL)ntIndex,
     fdep, depCount);
}
  }


 for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)
 {
   if (depCount[ntIndex] > 0)
{
  clearDependency(ntIndex, fdep, depCount, &ff);

    }
   }


  for (int ntIndex = 0; ntIndex < NON_TERMINAL_COUNT; ntIndex++)
{
free(fdep[ntIndex]);

   }
   free(fdep);

 return ff;
}


   parseTree *parseInputSourceCodeStream(table T, FirstAndFollow ff, grammar G, FILE *fp)

{
(void)ff;

    grammar_element *stack[200];
    parseTree *treeStack[200];

     twinBuffer B = (twinBuffer)malloc(sizeof(TWIN_BUFFER));
     for (int idx = 0; idx < 2 * BUFFER_SIZE; idx++)
    {
   B->buffer[idx] = '\0';

    }
B->index = 2 * BUFFER_SIZE - 1;
B->line = 1;
  populate_buffer(B, fp);
 B->index = 0;
 populate_buffer(B, fp);

  initializeLookupTable();


int sTop = 0;
  int tTop = 0;
int oldLine = -1;
   bool err = false;

   parseTree *PT = malloc(sizeof(parseTree));
   PT->ele.symbol.terminal = false;
  PT->ele.symbol.var.nt = NT_PROGRAM;
PT->ele.line = -1;
   PT->parent = NULL;


   treeStack[tTop] = PT;

  grammar_element *dollar = (grammar_element *)malloc(sizeof(grammar_element));
dollar->terminal = true;
  dollar->var.t = DOLLAR;
 grammar_element *program = (grammar_element *)malloc(sizeof(grammar_element));
 program->terminal = false;
program->var.nt = NT_PROGRAM;

stack[sTop] = dollar;
sTop++;
  stack[sTop] = program;

   tokenInfo a = nextToken(B, fp);
while (B->buffer[B->index] != '\0' && sTop >= 0)
{
grammar_element *X = stack[sTop];

    if (X->terminal)
{
  if (X->var.t == DOLLAR && a->type == DOLLAR)
       {
   break;
      }

 else if (X->var.t == a->type)
{
 parseTree *node = treeStack[tTop];
     node->ele.symbol.terminal = true;
    node->ele.symbol.var.t = a->type;
       node->ele.line = a->line;
  node->ele.lexeme = a->lexeme;
       node->ele.lexemeSize = a->lexemeSize;

  free(X);
  stack[sTop] = NULL;

     sTop--;
  tTop--;
       free(a);
  a = nextToken(B, fp);
 }
else
 {
err = true;
    if (oldLine == a->line)
   {
      free(a);

      a = nextToken(B, fp);
       continue;
     }

oldLine = a->line;
printf("Line %02d: Syntax Error : The token %s for lexeme %s does "
 "not "
 "match with the expected token %s\n",
a->line, getTokenName(a->type), a->lexeme,
getTokenName(X->var.t));
free(X);
 stack[sTop] = NULL;
 sTop--;
tTop--;

 }
 }
else
 {

     NON_TERMINAL nt = X->var.nt;
  int rule_no = T.table[nt][a->type];

   if (rule_no == -1)
{
    err = true;
  if (oldLine == a->line)
    {
       free(a);
    a = nextToken(B, fp);
     continue;

   }



   oldLine = a->line;
 printf("Line %02d: Syntax Error : Invalid token %s encountered "
    "with "
"value %s stack top %s\n",
     a->line, getTokenName(a->type), a->lexeme,
getNonTerminal(nt));
     free(a);
a = nextToken(B, fp);
  }
   else if (rule_no == -2)
{
     err = true;
if (oldLine == a->line)
    {
   free(a);
      a = nextToken(B, fp);
  continue;
  }


  oldLine = a->line;
printf("Line %02d: Syntax Error : Invalid token %s encountered "
"with "
"value %s stack top %s\n",
a->line, getTokenName(a->type), a->lexeme,
getNonTerminal(nt));
 free(X);
   stack[sTop] = NULL;
 sTop--;
   tTop--;
}
 else
 {
grammar_rule rule = G.rules[nt][rule_no];
     parseTree *node = treeStack[tTop];
     tTop--;
 free(X);
     stack[sTop] = NULL;
    sTop--;


   if (G.has_epsillon[nt] && rule_no == G.rule_count[nt])
     {
      node->no_of_children = 1;
       parseTree *ch = (parseTree *)malloc(sizeof(parseTree));
   ch->ele.symbol.terminal = true;
    ch->ele.symbol.var.t = EPSILLON;
 ch->ele.line = -1;
 ch->parent = node;
      ch->no_of_children = 0;
  node->children[0] = ch;

}
  else

  {
   node->no_of_children = rule.element_count;

for (int idx = rule.element_count - 1; idx >= 0; idx--)
     {
 parseTree *ch = (parseTree *)malloc(sizeof(parseTree));
 ch->ele.symbol.terminal = rule.elements[idx].terminal;
        ch->ele.line = -1;

 if (rule.elements[idx].terminal)
       {
   ch->ele.symbol.var.t = rule.elements[idx].var.t;
    }
  else
{
      ch->ele.symbol.var.nt = rule.elements[idx].var.nt;
}

ch->parent = node;
 ch->no_of_children = 0;
  node->children[idx] = ch;

   treeStack[++tTop] = ch;
   grammar_element *ele =
  (grammar_element *)malloc(sizeof(grammar_element));
  ele->terminal = rule.elements[idx].terminal;
   if (ele->terminal)
{
    ele->var.t = rule.elements[idx].var.t;

}
else


{
     ele->var.nt = rule.elements[idx].var.nt;

}

    stack[++sTop] = ele;
  }
   }

}

}
}

if (!(sTop == 0 && stack[sTop]->var.t == DOLLAR))
{
err = true;
   printf("Syntax Error : Tokens parsed but stack not empty\n");

}
 else if (a->type != DOLLAR)

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


while (sTop >= 0)

{
   if (stack[sTop] != NULL)
  {
     free(stack[sTop]);

}

 sTop--;
}


  return PT;
   }

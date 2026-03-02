/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/
#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"
#include <stdio.h>

trie createTrieNode();
   void insert(trie root, char *key, TOKEN_TYPE tokenType);
TOKEN_TYPE search(trie root, char *key);

FILE *getStream(FILE *fp);
 tokenInfo getNextToken(twinBuffer B);
  void removeComments(char *inputFile, char *outputFile);
 tokenInfo nextToken(twinBuffer B, FILE *fp);
char *getTokenName(TOKEN_TYPE type);
 void populate_buffer(twinBuffer B, FILE *fp);

void initializeLookupTable();
 bool handle_valid_error(tokenInfo token);
void resetLexicalErrorCount(void);
   int getLexicalErrorCount(void);


#endif

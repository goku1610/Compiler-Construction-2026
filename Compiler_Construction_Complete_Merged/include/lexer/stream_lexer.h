
#ifndef STREAM_LEXER_H
#define STREAM_LEXER_H

#include "core/token_model.h"
#include <stdio.h>

void getStream(FILE *fp);
tokenInfo getNextToken(twinBuffer B, FILE *fp);
void removeComments(char *inputFile, char *outputFile);
tokenInfo nextToken(twinBuffer B, FILE *fp);
char *getTokenName(TOKEN_TYPE type);
void populate_buffer(twinBuffer B, FILE *fp);
void initializeLookupTable();
bool handle_valid_error(tokenInfo token);
void resetLexicalErrorCount(void);
int getLexicalErrorCount(void);

#endif

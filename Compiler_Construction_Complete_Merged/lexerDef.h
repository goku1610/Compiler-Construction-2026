/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/
#ifndef LEXER_DEF_H
#define LEXER_DEF_H

#include "token_model.h"

#define ALPHABET_SIZE 26

typedef struct trie

  {
struct trie *children[ALPHABET_SIZE];
TOKEN_TYPE tokenType;
 } Trie;
  typedef Trie *trie;

#endif


#include "lexer/keyword_bank.h"
#include <stdlib.h>

trie createTrieNode()
{
    trie node = (trie)malloc(sizeof(Trie));
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        node->children[i] = NULL;
    }
    node->tokenType = TK_FIELDID;
    return node;
}

void insert(trie root, char* key, TOKEN_TYPE tokenType)
{
    trie temp = root;
    for (int i = 0; key[i] != '\0'; i++)
    {
        int index = key[i] - 'a';
        if (temp->children[index] == NULL)
        {
            temp->children[index] = createTrieNode();
        }
        temp = temp->children[index];
    }
    temp->tokenType = tokenType;
}

TOKEN_TYPE search(trie root, char* key)
{
    trie temp = root;
    for (int i = 0; key[i] != '\0'; i++)
    {
        int index = key[i] - 'a';
        if (temp->children[index] == NULL)
        {
            return TK_FIELDID;
        }
        temp = temp->children[index];
    }
    return temp->tokenType;
}
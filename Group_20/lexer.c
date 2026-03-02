/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/
#include "lexerDef.h"
#include "ascii_text.h"
#include <stdio.h>
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


  void insert(trie root, char *key, TOKEN_TYPE tokenType)
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

   TOKEN_TYPE search(trie root, char *key)

{
 trie temp = root;
 for (int i = 0; key[i] != '\0'; i++)
   {
   if (key[i] < 'a' || key[i] > 'z')
     {
     return TK_FIELDID;

        }
     int index = key[i] - 'a';
    if (temp->children[index] == NULL)

   {
      return TK_FIELDID;
   }
temp = temp->children[index];
   }

return temp->tokenType;
   }


  static trie keywordsLookupTable;
static int lexicalErrorCount = 0;
   static FILE *activeSourceFile = NULL;


#define STATE_INFO_OK(next_state, returning, token_type, redact)                   \
(STATE_INFO){(next_state), (returning), (token_type), (redact), 0}
#define STATE_INFO_ERR(next_state, returning, token_type, redact, err_code)        \
   (STATE_INFO)                                                                   \
 {                                                                              \
   (next_state), (returning), (token_type), (redact), (err_code)              \
   }


    void resetLexicalErrorCount(void)
{
     lexicalErrorCount = 0;
   }


int getLexicalErrorCount(void)

 {
 return lexicalErrorCount;

}



STATE_INFO getNextState(STATE currentState, char nextSymbol)
  {
switch (currentState)
{
   case START:
      {
    if (nextSymbol == ';')

     {
      return STATE_INFO_OK(START, true, TK_SEM, 0);
      }

   else if (nextSymbol == ',')
        {
        return STATE_INFO_OK(START, true, TK_COMMA, 0);
    }
    else if (nextSymbol == '.')
  {
 return STATE_INFO_OK(START, true, TK_DOT, 0);
   }

else if (nextSymbol == '(')
  {
 return STATE_INFO_OK(START, true, TK_OP, 0);

 }
  else if (nextSymbol == ')')
{
 return STATE_INFO_OK(START, true, TK_CL, 0);
 }
else if (nextSymbol == '[')

{
    return STATE_INFO_OK(START, true, TK_SQL, 0);
}
else if (nextSymbol == ']')
{
    return STATE_INFO_OK(START, true, TK_SQR, 0);
  }
else if (nextSymbol == '*')
    {
  return STATE_INFO_OK(START, true, TK_MUL, 0);
  }
 else if (nextSymbol == '/')

{
  return STATE_INFO_OK(START, true, TK_DIV, 0);
 }
  else if (nextSymbol == '+')
  {
 return STATE_INFO_OK(START, true, TK_PLUS, 0);

}
 else if (nextSymbol == '-')

  {
    return STATE_INFO_OK(START, true, TK_MINUS, 0);
}
else if (nextSymbol == '~')

{
     return STATE_INFO_OK(START, true, TK_NOT, 0);
}
else if (nextSymbol == '@')
{
      return STATE_INFO_OK(S13, false, NULL_TOKEN, 0);

}
else if (nextSymbol == '!')
{
    return STATE_INFO_OK(S16, false, NULL_TOKEN, 0);

  }

else if (nextSymbol == '&')
{
return STATE_INFO_OK(S18, false, NULL_TOKEN, 0);

}
else if (nextSymbol == '=')
{
    return STATE_INFO_OK(S21, false, NULL_TOKEN, 0);
}
else if (nextSymbol == '%')
   {
  return STATE_INFO_OK(S23, false, NULL_TOKEN, 0);
}
else if (nextSymbol == EOF)
  {
  return STATE_INFO_OK(START, true, EXIT_TOKEN, 0);

   }
else if (nextSymbol == ':')
{
return STATE_INFO_OK(START, true, TK_COLON, 0);
   }
 else if (nextSymbol == '<')

{
   return STATE_INFO_OK(S26, false, NULL_TOKEN, 0);
}
  else if (nextSymbol == '>')
  {
      return STATE_INFO_OK(S33, false, NULL_TOKEN, 0);
  }
   else if (nextSymbol == '_')
  {
 return STATE_INFO_OK(S37, false, NULL_TOKEN, 0);
  }
else if (nextSymbol == '#')
{

     return STATE_INFO_OK(S41, false, NULL_TOKEN, 0);

  }

 else if (nextSymbol >= '0' && nextSymbol <= '9')
{
return STATE_INFO_OK(S44, false, NULL_TOKEN, 0);
}
   else if (nextSymbol >= 'b' && nextSymbol <= 'd')
{
   return STATE_INFO_OK(S57, false, NULL_TOKEN, 0);
   }
  else if ((nextSymbol >= 'a' && nextSymbol <= 'z') ||
 (nextSymbol >= 'A' && nextSymbol <= 'Z'))

{
   return STATE_INFO_OK(S55, false, NULL_TOKEN, 0);
   }
else if (nextSymbol == '\t' || nextSymbol == ' ')
{
    return STATE_INFO_OK(START, true, BLANK, 0);
}
else if (nextSymbol == '\n')

{
     return STATE_INFO_OK(START, true, NEWLINE, 0);
 }
  else if (nextSymbol == '\0')
{
  return STATE_INFO_OK(START, true, BLANK, 0);
}

else
  {
return STATE_INFO_OK(INVALID, false, NULL_TOKEN, 0);
}
 }
 case S13:

 {
    if (nextSymbol == '@')
return STATE_INFO_OK(S14, false, NULL_TOKEN, 0);
   else
   return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 1);
  }

case S14:

   {
     if (nextSymbol == '@')
return STATE_INFO_OK(START, true, TK_OR, 0);
else
 return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 1);
   }
case S16:
 {
  if (nextSymbol == '=')
  return STATE_INFO_OK(START, true, TK_NE, 0);
     else
     return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 2);

}
case S18:

{
if (nextSymbol == '&')
return STATE_INFO_OK(S19, false, NULL_TOKEN, 0);
  else
     return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 3);
}
case S19:
   {
     if (nextSymbol == '&')
  return STATE_INFO_OK(START, true, TK_AND, 0);
else
return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 3);
  }

case S21:

 {
  if (nextSymbol == '=')
    return STATE_INFO_OK(START, true, TK_EQ, 0);
 else
 return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 4);
}
 case S23:

{
  if (nextSymbol != '\n' && nextSymbol != '\0')
 return STATE_INFO_OK(S23, false, NULL_TOKEN, 0);
   else
return STATE_INFO_OK(START, true, TK_COMMENT, 0);


}

  case S26:
{

      if (nextSymbol == '-')
  return STATE_INFO_OK(S28, false, NULL_TOKEN, 0);
     else if (nextSymbol == '=')
     return STATE_INFO_OK(START, true, TK_LE, 0);
   else
    return STATE_INFO_OK(START, true, TK_LT, 1);
  }

 case S28:

{
    if (nextSymbol == '-')
  return STATE_INFO_OK(S29, false, NULL_TOKEN, 0);
      else

return STATE_INFO_OK(START, true, TK_LT, 2);
   }
case S29:

  {
 if (nextSymbol == '-')
   return STATE_INFO_OK(START, true, TK_ASSIGNOP, 0);
    else
  return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 5);

  }
case S33:
 {
 if (nextSymbol == '=')
  return STATE_INFO_OK(START, true, TK_GE, 0);
else
     return STATE_INFO_OK(START, true, TK_GT, 1);


  }

case S37:
{
   if ((nextSymbol >= 'a' && nextSymbol <= 'z') ||
(nextSymbol >= 'A' && nextSymbol <= 'Z'))
  {
    return STATE_INFO_OK(S38, false, NULL_TOKEN, 0);
}
 else
{

     return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 6);

}
}
case S38:
{
 if ((nextSymbol >= 'a' && nextSymbol <= 'z') ||
   (nextSymbol >= 'A' && nextSymbol <= 'Z'))
 {
       return STATE_INFO_OK(S38, false, NULL_TOKEN, 0);

}

  else if (nextSymbol >= '0' && nextSymbol <= '9')

  {
return STATE_INFO_OK(S40, false, NULL_TOKEN, 0);
}
else
{
     return STATE_INFO_OK(START, true, TK_FUNID, 1);
  }
}
case S40:

{
if (nextSymbol >= '0' && nextSymbol <= '9')
{
      return STATE_INFO_OK(S40, false, NULL_TOKEN, 0);
 }

else

{
   return STATE_INFO_OK(START, true, TK_FUNID, 1);

}

}
case S41:
{
if (nextSymbol >= 'a' && nextSymbol <= 'z')
     {
      return STATE_INFO_OK(S42, false, NULL_TOKEN, 0);


     }
else
   {
   return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 7);

 }
}
 case S42:
{
     if (nextSymbol >= 'a' && nextSymbol <= 'z')
   {
   return STATE_INFO_OK(S42, false, NULL_TOKEN, 0);
    }

else
{
  return STATE_INFO_OK(START, true, TK_RUID, 1);
   }
}

case S44:
    {
if (nextSymbol >= '0' && nextSymbol <= '9')

{

      return STATE_INFO_OK(S44, false, NULL_TOKEN, 0);
}
 else if (nextSymbol == '.')
 {
     return STATE_INFO_OK(S46, false, NULL_TOKEN, 0);
}
else
{
return STATE_INFO_OK(START, true, TK_NUM, 1);

  }
}
case S46:
   {
    if (nextSymbol >= '0' && nextSymbol <= '9')
   {
     return STATE_INFO_OK(S47, false, NULL_TOKEN, 0);
    }
 else
   {
   return STATE_INFO_OK(START, true, TK_ERROR, 1);

  }

 }
 case S47:
{
     if (nextSymbol >= '0' && nextSymbol <= '9')
{
     return STATE_INFO_OK(S48, false, NULL_TOKEN, 0);
   }

 else
  {
return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 8);

}
}
 case S48:
    {
    if (nextSymbol == 'E')
{
     return STATE_INFO_OK(S50, false, NULL_TOKEN, 0);
}
else
  {
 return STATE_INFO_OK(START, true, TK_RNUM, 1);
  }
}

case S50:
{
if (nextSymbol == '+' || nextSymbol == '-')
   {
     return STATE_INFO_OK(S51, false, NULL_TOKEN, 0);
   }
   else if (nextSymbol >= '0' && nextSymbol <= '9')
   {
    return STATE_INFO_OK(S52, false, NULL_TOKEN, 0);
}
    else
   {
return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 9);
  }

 }
case S51:
{
  if (nextSymbol >= '0' && nextSymbol <= '9')
    {
 return STATE_INFO_OK(S52, false, NULL_TOKEN, 0);
 }
else

{
 return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 10);
   }
}
case S52:

 {
  if (nextSymbol >= '0' && nextSymbol <= '9')

   {
 return STATE_INFO_OK(START, true, TK_RNUM, 0);

   }

else
 {
return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 11);
}
}
case S55:
 {
    if (nextSymbol >= 'a' && nextSymbol <= 'z')

   {
    return STATE_INFO_OK(S55, false, NULL_TOKEN, 0);
 }
else
  {
return STATE_INFO_OK(START, true, TK_FIELDID, 1);

}

}
case S57:
{
     if (nextSymbol >= 'a' && nextSymbol <= 'z')
     {
   return STATE_INFO_OK(S55, false, NULL_TOKEN, 0);
  }
   else if (nextSymbol >= '2' && nextSymbol <= '7')
{
return STATE_INFO_OK(S58, false, NULL_TOKEN, 0);
}
  else
   {
  return STATE_INFO_OK(START, true, TK_FIELDID, 1);

 }
   }
 case S58:
 {
     if (nextSymbol >= '2' && nextSymbol <= '7')

     {
  return STATE_INFO_OK(S59, false, NULL_TOKEN, 0);

 }
else if (nextSymbol >= 'b' && nextSymbol <= 'd')
    {
return STATE_INFO_OK(S58, false, NULL_TOKEN, 0);
  }
 else
{
 return STATE_INFO_OK(START, true, TK_ID, 1);
}

}
case S59:
 {

    if (nextSymbol >= '2' && nextSymbol <= '7')
 {
     return STATE_INFO_OK(S59, false, NULL_TOKEN, 0);
 }
  else
{
return STATE_INFO_OK(START, true, TK_ID, 1);

}
  }
 default:
{

 return STATE_INFO_OK(INVALID, false, NULL_TOKEN, 0);
   }
   }

}



void removeComments(char *inputFile, char *outputFile)
  {
FILE *in = fopen(inputFile, "r");
  if (in == NULL)
  {
     printf("Error: Unable to open input file %s\n", inputFile);
    return;
}


 FILE *out = fopen(outputFile, "w");
if (out == NULL)

{
    printf("Error: Unable to open output file %s\n", outputFile);
fclose(in);
     return;
}


int ch = fgetc(in);
   while (ch != EOF)
{
     if (ch == '%')
 {
 while (ch != '\n' && ch != EOF)

  {
     ch = fgetc(in);
       }
 if (ch == '\n')
   {
  fputc('\n', out);
     ch = fgetc(in);


     }

}
 else

  {
   fputc(ch, out);
 ch = fgetc(in);
 }

 }



fclose(in);
fclose(out);

}

  void initializeLookupTable()
 {
 keywordsLookupTable = createTrieNode();
   insert(keywordsLookupTable, "as", TK_AS);
   insert(keywordsLookupTable, "call", TK_CALL);
insert(keywordsLookupTable, "definetype", TK_DEFINETYPE);
 insert(keywordsLookupTable, "else", TK_ELSE);
insert(keywordsLookupTable, "end", TK_END);
 insert(keywordsLookupTable, "endunion", TK_ENDUNION);
  insert(keywordsLookupTable, "endif", TK_ENDIF);
insert(keywordsLookupTable, "endrecord", TK_ENDRECORD);
    insert(keywordsLookupTable, "endwhile", TK_ENDWHILE);
insert(keywordsLookupTable, "global", TK_GLOBAL);
insert(keywordsLookupTable, "if", TK_IF);
insert(keywordsLookupTable, "input", TK_INPUT);
 insert(keywordsLookupTable, "int", TK_INT);
   insert(keywordsLookupTable, "list", TK_LIST);
  insert(keywordsLookupTable, "output", TK_OUTPUT);
     insert(keywordsLookupTable, "parameter", TK_PARAMETER);
insert(keywordsLookupTable, "parameters", TK_PARAMETERS);
insert(keywordsLookupTable, "read", TK_READ);
    insert(keywordsLookupTable, "real", TK_REAL);
     insert(keywordsLookupTable, "record", TK_RECORD);
     insert(keywordsLookupTable, "return", TK_RETURN);
    insert(keywordsLookupTable, "then", TK_THEN);
     insert(keywordsLookupTable, "type", TK_TYPE);
insert(keywordsLookupTable, "union", TK_UNION);
     insert(keywordsLookupTable, "while", TK_WHILE);
      insert(keywordsLookupTable, "with", TK_WITH);
 insert(keywordsLookupTable, "write", TK_WRITE);
}

char *getTokenName(TOKEN_TYPE type)
{
  switch (type)
 {
    case TK_ERROR:
      return "TK_ERROR";
case TK_ASSIGNOP:
       return "TK_ASSIGNOP";
   case TK_COMMENT:
  return "TK_COMMENT";
case TK_FIELDID:
    return "TK_FIELDID";
    case TK_ID:
 return "TK_ID";
   case TK_NUM:
       return "TK_NUM";
     case TK_RNUM:
    return "TK_RNUM";
     case TK_FUNID:
    return "TK_FUNID";
case TK_RUID:
       return "TK_RUID";
  case TK_WITH:
  return "TK_WITH";
case TK_PARAMETERS:
 return "TK_PARAMETERS";
  case TK_END:
 return "TK_END";
case TK_WHILE:
    return "TK_WHILE";
   case TK_UNION:
   return "TK_UNION";
    case TK_ENDUNION:
 return "TK_ENDUNION";
   case TK_DEFINETYPE:
    return "TK_DEFINETYPE";
     case TK_AS:
      return "TK_AS";
 case TK_TYPE:
     return "TK_TYPE";
case TK_MAIN:
     return "TK_MAIN";
case TK_GLOBAL:
       return "TK_GLOBAL";
   case TK_PARAMETER:
      return "TK_PARAMETER";
    case TK_LIST:
     return "TK_LIST";
case TK_SQL:
    return "TK_SQL";
case TK_SQR:
   return "TK_SQR";
  case TK_INPUT:
     return "TK_INPUT";
  case TK_OUTPUT:
       return "TK_OUTPUT";
    case TK_INT:
       return "TK_INT";
     case TK_REAL:
       return "TK_REAL";
 case TK_COMMA:
    return "TK_COMMA";
case TK_SEM:
  return "TK_SEM";
   case TK_COLON:
     return "TK_COLON";
  case TK_DOT:

     return "TK_DOT";
  case TK_ENDWHILE:
 return "TK_ENDWHILE";
 case TK_OP:
     return "TK_OP";
    case TK_CL:
    return "TK_CL";

  case TK_IF:
   return "TK_IF";
 case TK_THEN:

  return "TK_THEN";
case TK_ENDIF:

    return "TK_ENDIF";
    case TK_READ:

  return "TK_READ";
      case TK_WRITE:
  return "TK_WRITE";
     case TK_RETURN:
  return "TK_RETURN";
case TK_PLUS:
    return "TK_PLUS";
 case TK_MINUS:
        return "TK_MINUS";
   case TK_MUL:
      return "TK_MUL";
 case TK_DIV:
      return "TK_DIV";
     case TK_CALL:
 return "TK_CALL";
 case TK_RECORD:
     return "TK_RECORD";
    case TK_ENDRECORD:
      return "TK_ENDRECORD";
    case TK_ELSE:
 return "TK_ELSE";
 case TK_AND:
   return "TK_AND";
   case TK_OR:
      return "TK_OR";
    case TK_NOT:
       return "TK_NOT";

case TK_LT:
       return "TK_LT";
 case TK_LE:
      return "TK_LE";
case TK_EQ:
   return "TK_EQ";
case TK_GT:
       return "TK_GT";
   case TK_GE:
 return "TK_GE";
   case TK_NE:
 return "TK_NE";
case EPSILLON:
  return "EPSILLON";
    default:
    return "INVALID";
    }

 }

void printbuffer(twinBuffer B)

 {
for (int i = 0; i < 2 * BUFFER_SIZE; i++)
 {
   printf("%c ", B->buffer[i]);
  }
  printf("\n");
}

void populate_buffer(twinBuffer B, FILE *fp)
{

     if (B->index >= BUFFER_SIZE)
{
       for (int i = 0; i < BUFFER_SIZE; i++)
        {
     char ch = fgetc(fp);
    if (ch == EOF)

       {
       for (int j = i; j < BUFFER_SIZE; j++)

       {
          B->buffer[j] = '\0';
       }
 }
   else
 {
       B->buffer[i] = ch;

   }

   }
   }
   else
{

for (int i = BUFFER_SIZE; i < 2 * BUFFER_SIZE; i++)
     {
      char ch = fgetc(fp);
    if (ch == EOF)
 {
        for (int j = i; j < 2 * BUFFER_SIZE; j++)
      {
     B->buffer[j] = '\0';
       }
   }

else
  {
 B->buffer[i] = ch;
 }
 }
}
}


void handle_comments(twinBuffer B, FILE *fp)
 {
int before = B->index;
  while (B->buffer[B->index] != '\n' && B->buffer[B->index] != '\0')
    {
    B->index++;
       B->index = B->index % (2 * BUFFER_SIZE);
    int after = B->index;
   if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
  (before >= BUFFER_SIZE && after < BUFFER_SIZE))
    {
       populate_buffer(B, fp);
  }
before = B->index;

  }

 B->index++;
B->index = B->index % (2 * BUFFER_SIZE);
   int after = B->index;
if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
(before >= BUFFER_SIZE && after < BUFFER_SIZE))
{
populate_buffer(B, fp);
  }
    return;

}

void handle_invalid_error(STATE_INFO state, twinBuffer B, int start, int end)

{
    lexicalErrorCount++;
  char pattern[2 * BUFFER_SIZE + 1];
     int p = 0;

if (start == end)
    {
 pattern[p++] = B->buffer[start];
      pattern[p] = '\0';
       printf("Line No %d : Error: Unknown Symbol <%s>\n", B->line, pattern);
       B->index = (end + 1) % (2 * BUFFER_SIZE);
       return;
 }


 while (start != end)
{
   pattern[p++] = B->buffer[start];
    start++;
     start = start % (2 * BUFFER_SIZE);
  }
 pattern[p] = '\0';
B->index = end;

 if (state.error == 4)
{
printf("Line No %d: Error : Unknown Symbol <%s>\n", B->line, pattern);
}
else
 {
printf("Line no: %d : Error: Unknown pattern <%s>\n", B->line, pattern);
}

   }

bool handle_valid_error(tokenInfo token)
 {
if (token->type == TK_ERROR)
  {
  lexicalErrorCount++;
   printf("Line no: %d : Error: Unknown pattern <%s>\n", token->line,
  token->lexeme);
  return true;
 }

   if (token->type == TK_ID)
{
 if (token->lexemeSize > 20)
  {
     lexicalErrorCount++;
      printf("Line No %d: Error :Variable Identifier is longer than the "
       "prescribed length of 20 characters.\n",
 token->line);
       free(token->lexeme);
   free(token);
    return false;

  }
return true;
  }
else if (token->type == TK_FUNID)

{
    if (token->lexemeSize > 30)
  {
 lexicalErrorCount++;
    printf("Line No %d: Error :Function Identifier is longer than the "
 "prescribed length of 30 characters.\n",
    token->line);
     free(token->lexeme);
free(token);
   return false;
}

 return true;
  }

   return true;
}

  tokenInfo getNextToken(twinBuffer B)
{
  STATE currentState = START;

  if (B->buffer[B->index] == '%')
  {
     handle_comments(B, activeSourceFile);
    char *lexeme = (char *)malloc(sizeof(char) * 2);
       lexeme[0] = '%';

       lexeme[1] = '\0';
     tokenInfo token = (tokenInfo)malloc(sizeof(TOKEN));
    token->lexeme = lexeme;
 token->lexemeSize = 1;
    token->line = B->line;
 token->type = TK_COMMENT;
 return token;
}


 int start = B->index;
 int end = B->index;
    STATE_INFO nextState = getNextState(currentState, B->buffer[start]);
while (!(nextState.isReturningToken || nextState.nextSTATE == INVALID))
  {
end++;
    end = end % (2 * BUFFER_SIZE);

 nextState = getNextState(nextState.nextSTATE, B->buffer[end]);
}


if (nextState.nextSTATE == INVALID)

 {
     handle_invalid_error(nextState, B, start, end);
   return NULL;
 }
else if (nextState.tokenType == BLANK)
  {


   end++;
end = end % (2 * BUFFER_SIZE);
 B->index = end;
 return NULL;

 }
else if (nextState.tokenType == NEWLINE)
{

end++;
  end = end % (2 * BUFFER_SIZE);
   B->index = end;
B->line++;

  return NULL;

  }
else
 {
     int redaction = nextState.redaction;

   end = (end - redaction + 2 * BUFFER_SIZE) % (2 * BUFFER_SIZE);
    int size = 0;
 if (end >= start)

     {
        size = end - start + 1;

}
 else
   {
size = 2 * BUFFER_SIZE - start + end + 1;
  }

char *lexeme = (char *)malloc(sizeof(char) * (size + 1));
int i = 0;
while (start != end)

  {
    lexeme[i] = B->buffer[start];
start++;
 start = start % (2 * BUFFER_SIZE);
     i++;

   }

lexeme[i] = B->buffer[start];
lexeme[i + 1] = '\0';
   start++;
 start = start % (2 * BUFFER_SIZE);
   tokenInfo token = (tokenInfo)malloc(sizeof(TOKEN));
token->lexeme = lexeme;
   token->lexemeSize = size;
 token->line = B->line;

   if (nextState.tokenType == TK_FIELDID)
{
token->type = search(keywordsLookupTable, lexeme);
   }
 else if (nextState.tokenType == TK_FUNID)
   {

     if (stringcmp(lexeme, "_main"))

    {
      token->type = TK_MAIN;


}
else

  {
token->type = TK_FUNID;

   }

  }
 else
{
  token->type = nextState.tokenType;
}
  B->index = start;
   return token;
 }
}

 FILE *getStream(FILE *fp)
{
    resetLexicalErrorCount();
     activeSourceFile = fp;
 twinBuffer B = (twinBuffer)malloc(sizeof(TWIN_BUFFER));
for (int i = 0; i < 2 * BUFFER_SIZE; i++)
{
       B->buffer[i] = '\0';

  }

B->index = 2 * BUFFER_SIZE - 1;
 B->line = 1;
populate_buffer(B, fp);
B->index = 0;
populate_buffer(B, fp);
initializeLookupTable();
printf("%-12s%-30s%-20s\n", "LineNo", "Lexeme", "Token");
printf("%-12s%-30s%-20s\n", "------", "------", "-----");
   while (B->buffer[B->index] != '\0')

  {
 int before = B->index;
 tokenInfo token = getNextToken(B);
  if (token != NULL)

{
    if (token->type == NEWLINE || token->type == TK_COMMENT)

      {
   B->line++;
      }
 if (token->type != NULL_TOKEN && token->type != NEWLINE &&
token->type != EXIT_TOKEN && token->type != BLANK)

 {

    if (handle_valid_error(token))
  printf("%-12d%-30s%-20s\n", token->line, token->lexeme,
   getTokenName(token->type));
  }
if (token->type == TK_COMMENT)

  {
continue;

   }
}
 int after = B->index;
if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
  (before >= BUFFER_SIZE && after < BUFFER_SIZE))
{
      populate_buffer(B, fp);
 }
 }
 free(B);
   return fp;

}

   tokenInfo nextToken(twinBuffer B, FILE *fp)
{
  activeSourceFile = fp;
    int before = B->index;
  tokenInfo token = getNextToken(B);
 bool flag = false;
    if (token != NULL)

     {
   if (token->type == NEWLINE || token->type == TK_COMMENT)

      {
        B->line++;
      }

    if (token->type == TK_COMMENT)
  {
     if (B->buffer[B->index] != '\0')
   return nextToken(B, fp);
       else
      {
       tokenInfo dollarToken;
    dollarToken = (tokenInfo)malloc(sizeof(TOKEN));
       dollarToken->type = DOLLAR;
        return dollarToken;
      }
 }
if (token->type == TK_ERROR)
{
(void)handle_valid_error(token);
free(token->lexeme);
 free(token);
  token = NULL;

}
   else if (token->type != NULL_TOKEN && token->type != NEWLINE &&
  token->type != EXIT_TOKEN && token->type != BLANK)
 {
    if (handle_valid_error(token))

{
 flag = true;

    }
   }
 }
int after = B->index;
if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
(before >= BUFFER_SIZE && after < BUFFER_SIZE))
{
populate_buffer(B, fp);
}
if (flag)
  return token;
else

 {
   if (B->buffer[B->index] != '\0')
    return nextToken(B, fp);
    else

    {
      tokenInfo dollarToken;
   dollarToken = (tokenInfo)malloc(sizeof(TOKEN));
     dollarToken->type = DOLLAR;
       return dollarToken;

  }


   }
}

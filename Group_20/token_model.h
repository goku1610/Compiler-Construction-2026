/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/


#ifndef TOKEN_MODEL_H
#define TOKEN_MODEL_H

#include <stdbool.h>

#define BUFFER_SIZE 50
#define TOTAL_STATES 64
#define TOTAL_TOKENS 64

static const char alphabet[] = {
    'a', 'b', 'c', 'd',  'e',  'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
 'p', 'q', 'r', 's',  't',  'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
   'E', 'F', 'G', 'H',  'I',  'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
'T', 'U', 'V', 'W',  'X',  'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7',
 '8', '9', ' ', '\n', '\t', '~', '(', '[', ']', ')', '<', '>', '!', '@', '#',
 '%', '&', '*', '_',  '+',  '-', '/', '=', ';', ':', '.', ','};
 static const int alphabetSize = sizeof(alphabet) / sizeof(alphabet[0]);

typedef enum STATE
{
     START,
     S1,
    S2,
     S3,
     S4,
S5,
  S6,
   S7,
 S8,
S9,
     S10,
 S11,
   S12,
 S13,
S14,
S15,
 S16,
S17,
   S18,
    S19,
      S20,
      S21,
  S22,
 S23,
S24,
     S25,
   S26,
      S27,
S28,
   S29,
S30,
 S31,
  S32,
S33,
     S34,
  S35,
 S36,
 S37,
S38,
   S39,
   S40,
     S41,
      S42,
S43,
S44,
S45,
  S46,
    S47,
  S48,
S49,
 S50,
 S51,
     S52,
     S53,
  S54,
S55,
    S56,
 S57,
S58,

    S59,
    S60,
 S61,
     INVALID,
 EXIT,

  } STATE;


 typedef enum TOKEN_TYPE
   {
   TK_ASSIGNOP,
TK_COMMENT,
TK_FIELDID,
     TK_ID,
  TK_NUM,
  TK_RNUM,
   TK_FUNID,
     TK_RUID,
  TK_WITH,
TK_PARAMETERS,
TK_END,
  TK_WHILE,
TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
     TK_TYPE,
    TK_MAIN,
   TK_GLOBAL,
    TK_PARAMETER,
TK_LIST,
  TK_SQL,
TK_SQR,
  TK_INPUT,
 TK_OUTPUT,
     TK_INT,
TK_REAL,
  TK_COMMA,
   TK_SEM,
 TK_COLON,
 TK_DOT,
 TK_ENDWHILE,
   TK_OP,
  TK_CL,
 TK_IF,
TK_THEN,
     TK_ENDIF,
    TK_READ,
TK_WRITE,
 TK_RETURN,
TK_PLUS,
  TK_MINUS,
     TK_MUL,
TK_DIV,
TK_CALL,
   TK_RECORD,
     TK_ENDRECORD,
TK_ELSE,
 TK_AND,
   TK_OR,
    TK_NOT,
  TK_LT,
TK_LE,
    TK_EQ,
TK_GT,
 TK_GE,
    TK_NE,
NULL_TOKEN,
     NEWLINE,
 EXIT_TOKEN,
  BLANK,
EPSILLON,
    DOLLAR,
 TK_ERROR,
 } TOKEN_TYPE;

typedef struct TOKEN
   {
TOKEN_TYPE type;
char *lexeme;
     int lexemeSize;
 int line;

} TOKEN;


  typedef TOKEN *tokenInfo;


   typedef struct STATE_INFO
{
    STATE nextSTATE;
 bool isReturningToken;
   TOKEN_TYPE tokenType;
 int redaction;
   int error;
  } STATE_INFO;

typedef struct TWIN_BUFFER
 {
   char buffer[2 * BUFFER_SIZE];
int index;
    int line;
  } TWIN_BUFFER;

   typedef TWIN_BUFFER *twinBuffer;

#endif

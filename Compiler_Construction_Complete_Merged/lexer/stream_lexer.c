
#include "lexer/stream_lexer.h"
#include "shared/ascii_text.h"
#include "lexer/keyword_bank.h"
#include <stdbool.h>
#include <stdlib.h>

static trie keywordsLookupTable;
static int lexicalErrorCount = 0;

void resetLexicalErrorCount(void) {
  lexicalErrorCount = 0;
}

int getLexicalErrorCount(void) {
  return lexicalErrorCount;
}




STATE_INFO getNextState(STATE currentState, char nextSymbol) {
  switch (currentState) {
  case START: {
    if (nextSymbol == ';') {
      return (STATE_INFO){START, true, TK_SEM, 0};
    } else if (nextSymbol == ',') {
      return (STATE_INFO){START, true, TK_COMMA, 0};
    } else if (nextSymbol == '.') {
      return (STATE_INFO){START, true, TK_DOT, 0};
    } else if (nextSymbol == '(') {
      return (STATE_INFO){START, true, TK_OP, 0};
    } else if (nextSymbol == ')') {
      return (STATE_INFO){START, true, TK_CL, 0};
    } else if (nextSymbol == '[') {
      return (STATE_INFO){START, true, TK_SQL, 0};
    } else if (nextSymbol == ']') {
      return (STATE_INFO){START, true, TK_SQR, 0};
    } else if (nextSymbol == '*') {
      return (STATE_INFO){START, true, TK_MUL, 0};
    } else if (nextSymbol == '/') {
      return (STATE_INFO){START, true, TK_DIV, 0};
    } else if (nextSymbol == '+') {
      return (STATE_INFO){START, true, TK_PLUS, 0};
    } else if (nextSymbol == '-') {
      return (STATE_INFO){START, true, TK_MINUS, 0};
    } else if (nextSymbol == '~') {
      return (STATE_INFO){START, true, TK_NOT, 0};
    } else if (nextSymbol == '@') {
      return (STATE_INFO){S13, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '!') {
      return (STATE_INFO){S16, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '&') {
      return (STATE_INFO){S18, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '=') {
      return (STATE_INFO){S21, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '%') {
      return (STATE_INFO){S23, false, NULL_TOKEN, 0};
    } else if (nextSymbol == EOF) {
      return (STATE_INFO){START, true, EXIT_TOKEN, 0};
    } else if (nextSymbol == ':') {
      return (STATE_INFO){START, true, TK_COLON, 0};
    } else if (nextSymbol == '<') {
      return (STATE_INFO){S26, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '>') {
      return (STATE_INFO){S33, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '_') {
      return (STATE_INFO){S37, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '#') {
      return (STATE_INFO){S41, false, NULL_TOKEN, 0};
    } else if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S44, false, NULL_TOKEN, 0};
    } else if (nextSymbol >= 'b' && nextSymbol <= 'd') {
      return (STATE_INFO){S57, false, NULL_TOKEN, 0};
    } else if ((nextSymbol >= 'a' && nextSymbol <= 'z') ||
               (nextSymbol >= 'A' && nextSymbol <= 'Z')) {
      return (STATE_INFO){S55, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '\t' || nextSymbol == ' ') {
      return (STATE_INFO){START, true, BLANK, 0};
    } else if (nextSymbol == '\n') {
      return (STATE_INFO){START, true, NEWLINE, 0};
    } else if (nextSymbol == '\0') {
      return (STATE_INFO){START, true, BLANK, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0};
    }
  }
  case S13: {
    if (nextSymbol == '@')
      return (STATE_INFO){S14, false, NULL_TOKEN, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 1};
  }
  case S14: {
    if (nextSymbol == '@')
      return (STATE_INFO){START, true, TK_OR, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 1};
  }
  case S16: {
    if (nextSymbol == '=')
      return (STATE_INFO){START, true, TK_NE, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 2};
  }
  case S18: {
    if (nextSymbol == '&')
      return (STATE_INFO){S19, false, NULL_TOKEN, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 3};
  }
  case S19: {
    if (nextSymbol == '&')
      return (STATE_INFO){START, true, TK_AND, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 3};
  }
  case S21: {
    if (nextSymbol == '=')
      return (STATE_INFO){START, true, TK_EQ, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 4};
  }
  case S23: {
    if (nextSymbol != '\n' && nextSymbol != '\0')
      return (STATE_INFO){S23, false, NULL_TOKEN, 0};
    else
      return (STATE_INFO){START, true, TK_COMMENT, 0};
  }
  case S26: {
    if (nextSymbol == '-')
      return (STATE_INFO){S28, false, NULL_TOKEN, 0};
    else if (nextSymbol == '=')
      return (STATE_INFO){START, true, TK_LE, 0};
    else
      return (STATE_INFO){START, true, TK_LT, 1};
  }
  case S28: {
    if (nextSymbol == '-')
      return (STATE_INFO){S29, false, NULL_TOKEN, 0};
    else
      return (STATE_INFO){START, true, TK_LT, 2};
  }
  case S29: {
    if (nextSymbol == '-')
      return (STATE_INFO){START, true, TK_ASSIGNOP, 0};
    else
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 5};
  }
  case S33: {
    if (nextSymbol == '=')
      return (STATE_INFO){START, true, TK_GE, 0};
    else
      return (STATE_INFO){START, true, TK_GT, 1};
  }
  case S37: {
    if ((nextSymbol >= 'a' && nextSymbol <= 'z') ||
        (nextSymbol >= 'A' && nextSymbol <= 'Z')) {
      return (STATE_INFO){S38, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 6};
    }
  }
  case S38: {
    if ((nextSymbol >= 'a' && nextSymbol <= 'z') ||
        (nextSymbol >= 'A' && nextSymbol <= 'Z')) {
      return (STATE_INFO){S38, false, NULL_TOKEN, 0};
    } else if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S40, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_FUNID, 1};
    }
  }
  case S40: {
    if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S40, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_FUNID, 1};
    }
  }
  case S41: {
    if (nextSymbol >= 'a' && nextSymbol <= 'z') {
      return (STATE_INFO){S42, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 7};
    }
  }
  case S42: {
    if (nextSymbol >= 'a' && nextSymbol <= 'z') {
      return (STATE_INFO){S42, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_RUID, 1};
    }
  }
  case S44: {
    if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S44, false, NULL_TOKEN, 0};
    } else if (nextSymbol == '.') {
      return (STATE_INFO){S46, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_NUM, 1};
    }
  }
  case S46: {
    if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S47, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_NUM, 2};
    }
  }
  case S47: {
    if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S48, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 8};
    }
  }
  case S48: {
    if (nextSymbol == 'E') {
      return (STATE_INFO){S50, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_RNUM, 1};
    }
  }
  case S50: {
    if (nextSymbol == '+' || nextSymbol == '-') {
      return (STATE_INFO){S51, false, NULL_TOKEN, 0};
    } else if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S52, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 9};
    }
  }
  case S51: {
    if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){S52, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 10};
    }
  }
  case S52: {
    if (nextSymbol >= '0' && nextSymbol <= '9') {
      return (STATE_INFO){START, true, TK_RNUM, 0};
    } else {
      return (STATE_INFO){INVALID, false, NULL_TOKEN, 0, 11};
    }
  }
  case S55: {
    if (nextSymbol >= 'a' && nextSymbol <= 'z') {
      return (STATE_INFO){S55, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_FIELDID, 1};
    }
  }
  case S57: {
    if (nextSymbol >= 'a' && nextSymbol <= 'z') {
      return (STATE_INFO){S55, false, NULL_TOKEN, 0};
    } else if (nextSymbol >= '2' && nextSymbol <= '7') {
      return (STATE_INFO){S58, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_FIELDID, 1};
    }
  }
  case S58: {
    if (nextSymbol >= '2' && nextSymbol <= '7') {
      return (STATE_INFO){S59, false, NULL_TOKEN, 0};
    } else if (nextSymbol >= 'b' && nextSymbol <= 'd') {
      return (STATE_INFO){S58, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_ID, 1};
    }
  }
  case S59: {

    if (nextSymbol >= '2' && nextSymbol <= '7') {
      return (STATE_INFO){S59, false, NULL_TOKEN, 0};
    } else {
      return (STATE_INFO){START, true, TK_ID, 1};
    }
  }
  default: {
    
    return (STATE_INFO){INVALID, false, NULL_TOKEN, 0};
  }
  }
}


void initializeLookupTable() {
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


char *getTokenName(TOKEN_TYPE type) {
  switch (type) {
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


void populate_buffer(twinBuffer B, FILE *fp) {
  
  if (B->index >= BUFFER_SIZE) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
      char ch = fgetc(fp);
      if (ch == EOF) {
        for (int j = i; j < BUFFER_SIZE; j++) {
          B->buffer[j] = '\0';
        }
      } else {
        B->buffer[i] = ch;
      }
    }
  } else {
  
    for (int i = BUFFER_SIZE; i < 2 * BUFFER_SIZE; i++) {
      char ch = fgetc(fp);
      if (ch == EOF) {
        for (int j = i; j < 2 * BUFFER_SIZE; j++) {
          B->buffer[j] = '\0';
        }
      } else {
        B->buffer[i] = ch;
      }
    }
  }
}


void handle_comments(twinBuffer B, FILE *fp) {
  int before = B->index;
  while (B->buffer[B->index] != '\n' && B->buffer[B->index] != '\0') {
    B->index++;
    B->index = B->index % (2 * BUFFER_SIZE);
    int after = B->index;
    if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
        (before >= BUFFER_SIZE && after < BUFFER_SIZE)) {
      populate_buffer(B, fp);
    }
    before = B->index;
  }
  B->index++;
  B->index = B->index % (2 * BUFFER_SIZE);
  int after = B->index;
  if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
      (before >= BUFFER_SIZE && after < BUFFER_SIZE)) {
    populate_buffer(B, fp);
  }
  return;
}


void handle_invalid_error(STATE_INFO state, twinBuffer B, int start, int end) {
  lexicalErrorCount++;
  printf("Line %02d: Lexical Error: ", B->line);
  printf("Error: ");
  if (start == end) {
    printf("Unknown symbol <");
    printf("%c", B->buffer[start]);
    printf(">\n");
    B->index = (end + 1) % (2 * BUFFER_SIZE);
    return;
  }
  printf("Unknown pattern <");
  while (start != end) {
    printf("%c", B->buffer[start]);
    start++;
    start = start % (2 * BUFFER_SIZE);
  }
  printf("> ");
  B->index = end;
  int error_type = state.error;
  switch (error_type) {
  case 1: {
    printf(": Expected @@@\n");
    break;
  }
  case 2: {
    printf(": Expected !=\n");
    break;
  }
  case 3: {
    printf(": Expected &&&\n");
    break;
  }
  case 4: {
    printf(": Expected ==\n");
    break;
  }
  case 5: {
    printf(": Expected <---\n");
    break;
  }
  case 6: {
    printf(": Expected a letter [a-z]|[A-Z] after _\n");
    break;
  }
  case 7: {
    printf(": Expected a lowercase letter [a-z] after #\n");
    break;
  }
  case 8: {
    printf(": Expected a two digits [0-9] after decimal . but got one\n");
    break;
  }
  case 9: {
    printf(": Expected a digit [0-9] or +|- after E\n");
    break;
  }
  case 10: {
    printf(": Expected a digit [0-9] after +|-|E\n");
    break;
  }
  case 11: {
    printf(": Expected a two digits [0-9] after E|+|- but got one\n");
    break;
  }
  default: {
    printf("\n");
  }
  }
}


bool handle_valid_error(tokenInfo token) {
  if (token->type == TK_ID) {
    if (token->lexemeSize > 20) {
      lexicalErrorCount++;
      printf("Line %02d: Lexical Error: ", token->line);
      printf("Error: Variable Identifier %s is longer than the prescribed "
             "length of 20 characters\n",
             token->lexeme);
      free(token->lexeme);
      free(token);
      return false;
    }
    return true;
  } else if (token->type == TK_FUNID) {
    if (token->lexemeSize > 30) {
      lexicalErrorCount++;
      printf("Line %02d: Lexical Error: ", token->line);
      printf("Error: Function Identifier %s is longer than the prescribed "
             "length of 30 characters\n",
             token->lexeme);
      free(token->lexeme);
      free(token);
      return false;
    }
    return true;
  }
  return true;
}


tokenInfo getNextToken(twinBuffer B, FILE *fp) {
  STATE currentState = START;
  
  if (B->buffer[B->index] == '%') {
    handle_comments(B, fp);
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
  while (!(nextState.isReturningToken || nextState.nextSTATE == INVALID)) {
    end++;
    end = end % (2 * BUFFER_SIZE);
    nextState = getNextState(nextState.nextSTATE, B->buffer[end]);
  }
  
  if (nextState.nextSTATE == INVALID) {
    handle_invalid_error(nextState, B, start, end);
    return NULL;
  } else if (nextState.tokenType == BLANK) {
    
    end++;
    end = end % (2 * BUFFER_SIZE);
    B->index = end;
    return NULL;
  } else if (nextState.nextSTATE == NEWLINE) {
    
    end++;
    end = end % (2 * BUFFER_SIZE);
    B->index = end;
    B->line++;
    return NULL;
  } else {
    int redaction = nextState.redaction;
    end = (end - redaction + 2 * BUFFER_SIZE) % (2 * BUFFER_SIZE);
    int size = 0;
    if (end >= start) {
      size = end - start + 1;
    } else {
      size = 2 * BUFFER_SIZE - start + end + 1;
    }
    char *lexeme = (char *)malloc(sizeof(char) * (size + 1));
    int i = 0;
    while (start != end) {
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
    
    if (nextState.tokenType == TK_FIELDID) {
      token->type = search(keywordsLookupTable, lexeme);
    } else if (nextState.tokenType == TK_FUNID) {
      
      if (stringcmp(lexeme, "_main")) {
        token->type = TK_MAIN;
      } else {
        token->type = TK_FUNID;
      }
    } else {
      token->type = nextState.tokenType;
    }
    B->index = start;
    return token;
  }
}


void printbuffer(twinBuffer B) {
  for (int i = 0; i < 2 * BUFFER_SIZE; i++) {
    printf("%c ", B->buffer[i]);
  }
  printf("\n");
}


tokenInfo nextToken(twinBuffer B, FILE *fp) {
  int before = B->index;
  tokenInfo token = getNextToken(B, fp);
  bool flag = false;
  if (token != NULL) {
    if (token->type == NEWLINE || token->type == TK_COMMENT) {
      B->line++;
    }
    if (token->type == TK_COMMENT) {
      if (B->buffer[B->index] != '\0')
        return nextToken(B, fp);
      else {
        tokenInfo dollarToken;
        dollarToken = (tokenInfo)malloc(sizeof(TOKEN));
        dollarToken->type = DOLLAR;
        return dollarToken;
      }
    }
    if (token->type != NULL_TOKEN && token->type != NEWLINE &&
        token->type != EXIT_TOKEN && token->type != BLANK) {
      if (handle_valid_error(token))
        flag = true;
    }
  }
  int after = B->index;
  if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
      (before >= BUFFER_SIZE && after < BUFFER_SIZE)) {
    populate_buffer(B, fp);
  }
  if (flag)
    return token;
  else {
    if (B->buffer[B->index] != '\0')
      return nextToken(B, fp);
    else {
      tokenInfo dollarToken;
      dollarToken = (tokenInfo)malloc(sizeof(TOKEN));
      dollarToken->type = DOLLAR;
      return dollarToken;
    }
  }
}


void getStream(FILE *fp) {
  resetLexicalErrorCount();
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
  while (B->buffer[B->index] != '\0') {
    int before = B->index;
    tokenInfo token = getNextToken(B, fp);
    if (token != NULL) {
      if (token->type == NEWLINE || token->type == TK_COMMENT) {
        B->line++;
      }
      if (token->type != NULL_TOKEN && token->type != NEWLINE &&
          token->type != EXIT_TOKEN && token->type != BLANK) {
        if (handle_valid_error(token))
          printf("Line no. %d Lexeme %s Token %s\n", token->line, token->lexeme,
                 getTokenName(token->type));
      }
      if (token->type == TK_COMMENT) {
        continue;
      }
    }
    int after = B->index;
    if ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
        (before >= BUFFER_SIZE && after < BUFFER_SIZE)) {
      populate_buffer(B, fp);
    }
  }
  free(B);
}


void removeComments(char *inputFile, char *outputFile) {
  FILE *in = fopen(inputFile, "r");
  if (in == NULL) {
    printf("Error: Unable to open input file %s\n", inputFile);
    return;
  }

  FILE *out = fopen(outputFile, "w");
  if (out == NULL) {
    printf("Error: Unable to open output file %s\n", outputFile);
    fclose(in);
    return;
  }

  int ch = fgetc(in);
  while (ch != EOF) {
    if (ch == '%') {
      while (ch != '\n' && ch != EOF) {
        ch = fgetc(in);
      }
      if (ch == '\n') {
        fputc('\n', out);
        ch = fgetc(in);
      }
    } else {
      fputc(ch, out);
      ch = fgetc(in);
    }
  }

  fclose(in);
  fclose(out);
}

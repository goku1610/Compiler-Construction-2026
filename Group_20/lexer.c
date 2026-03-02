/*
Group Number: 20 | Team Members:
2022B3A70374P Aditya Thole | 2022B5A71313P Anmol Agrawal
2022B5A71614P Ansh Nohria | 2022B3A70512P Nirmal Swaroop Raghunandan
2022B2A70653P Saksham Rohatgi
*/

#include "lexerDef.h"
#include "ascii_text.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATE_INFO_OK(next_state, returning, token_type, redact) \
    (STATE_INFO){(next_state), (returning), (token_type), (redact), 0}
#define STATE_INFO_ERR(next_state, returning, token_type, redact, err_code) \
    (STATE_INFO){(next_state), (returning), (token_type), (redact), (err_code)}

typedef struct {
    const char *keyword;
    TOKEN_TYPE type;
} KeywordPair;

typedef struct {
    TOKEN_TYPE type;
    const char *name;
} TokenNamePair;

static const KeywordPair k_keywords[] = {
    {"as", TK_AS},
    {"call", TK_CALL},
    {"definetype", TK_DEFINETYPE},
    {"else", TK_ELSE},
    {"end", TK_END},
    {"endunion", TK_ENDUNION},
    {"endif", TK_ENDIF},
    {"endrecord", TK_ENDRECORD},
    {"endwhile", TK_ENDWHILE},
    {"global", TK_GLOBAL},
    {"if", TK_IF},
    {"input", TK_INPUT},
    {"int", TK_INT},
    {"list", TK_LIST},
    {"output", TK_OUTPUT},
    {"parameter", TK_PARAMETER},
    {"parameters", TK_PARAMETERS},
    {"read", TK_READ},
    {"real", TK_REAL},
    {"record", TK_RECORD},
    {"return", TK_RETURN},
    {"then", TK_THEN},
    {"type", TK_TYPE},
    {"union", TK_UNION},
    {"while", TK_WHILE},
    {"with", TK_WITH},
    {"write", TK_WRITE},
};

static const TokenNamePair k_token_names[] = {
    {TK_ERROR, "TK_ERROR"},
    {TK_ASSIGNOP, "TK_ASSIGNOP"},
    {TK_COMMENT, "TK_COMMENT"},
    {TK_FIELDID, "TK_FIELDID"},
    {TK_ID, "TK_ID"},
    {TK_NUM, "TK_NUM"},
    {TK_RNUM, "TK_RNUM"},
    {TK_FUNID, "TK_FUNID"},
    {TK_RUID, "TK_RUID"},
    {TK_WITH, "TK_WITH"},
    {TK_PARAMETERS, "TK_PARAMETERS"},
    {TK_END, "TK_END"},
    {TK_WHILE, "TK_WHILE"},
    {TK_UNION, "TK_UNION"},
    {TK_ENDUNION, "TK_ENDUNION"},
    {TK_DEFINETYPE, "TK_DEFINETYPE"},
    {TK_AS, "TK_AS"},
    {TK_TYPE, "TK_TYPE"},
    {TK_MAIN, "TK_MAIN"},
    {TK_GLOBAL, "TK_GLOBAL"},
    {TK_PARAMETER, "TK_PARAMETER"},
    {TK_LIST, "TK_LIST"},
    {TK_SQL, "TK_SQL"},
    {TK_SQR, "TK_SQR"},
    {TK_INPUT, "TK_INPUT"},
    {TK_OUTPUT, "TK_OUTPUT"},
    {TK_INT, "TK_INT"},
    {TK_REAL, "TK_REAL"},
    {TK_COMMA, "TK_COMMA"},
    {TK_SEM, "TK_SEM"},
    {TK_COLON, "TK_COLON"},
    {TK_DOT, "TK_DOT"},
    {TK_ENDWHILE, "TK_ENDWHILE"},
    {TK_OP, "TK_OP"},
    {TK_CL, "TK_CL"},
    {TK_IF, "TK_IF"},
    {TK_THEN, "TK_THEN"},
    {TK_ENDIF, "TK_ENDIF"},
    {TK_READ, "TK_READ"},
    {TK_WRITE, "TK_WRITE"},
    {TK_RETURN, "TK_RETURN"},
    {TK_PLUS, "TK_PLUS"},
    {TK_MINUS, "TK_MINUS"},
    {TK_MUL, "TK_MUL"},
    {TK_DIV, "TK_DIV"},
    {TK_CALL, "TK_CALL"},
    {TK_RECORD, "TK_RECORD"},
    {TK_ENDRECORD, "TK_ENDRECORD"},
    {TK_ELSE, "TK_ELSE"},
    {TK_AND, "TK_AND"},
    {TK_OR, "TK_OR"},
    {TK_NOT, "TK_NOT"},
    {TK_LT, "TK_LT"},
    {TK_LE, "TK_LE"},
    {TK_EQ, "TK_EQ"},
    {TK_GT, "TK_GT"},
    {TK_GE, "TK_GE"},
    {TK_NE, "TK_NE"},
    {EPSILLON, "EPSILLON"},
};

static int lexicalErrorCount = 0;
static FILE *activeSourceFile = NULL;
void populate_buffer(twinBuffer B, FILE *fp);

static int is_lower_alpha(char ch)
{
    return (ch >= 'a' && ch <= 'z');
}

static int is_alpha_any(char ch)
{
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

static int is_digit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

static TOKEN_TYPE keyword_lookup(const char *lexeme)
{
    if (lexeme == NULL)
    {
        return TK_FIELDID;
    }

    for (size_t idx = 0; idx < sizeof(k_keywords) / sizeof(k_keywords[0]); ++idx)
    {
        if (strcmp(lexeme, k_keywords[idx].keyword) == 0)
        {
            return k_keywords[idx].type;
        }
    }

    return TK_FIELDID;
}

static int crossed_buffer_half(int before, int after)
{
    return ((before < BUFFER_SIZE && after >= BUFFER_SIZE) ||
            (before >= BUFFER_SIZE && after < BUFFER_SIZE));
}

static void fill_buffer_half(twinBuffer B, FILE *fp, int from, int to)
{
    for (int idx = from; idx < to; ++idx)
    {
        int value = fgetc(fp);
        if (value == EOF)
        {
            for (int pad = idx; pad < to; ++pad)
            {
                B->buffer[pad] = '\0';
            }
            break;
        }
        B->buffer[idx] = (char)value;
    }
}

static void advance_with_refill(twinBuffer B, FILE *fp)
{
    int before = B->index;
    B->index = (B->index + 1) % (2 * BUFFER_SIZE);
    if (crossed_buffer_half(before, B->index))
    {
        populate_buffer(B, fp);
    }
}

static void collect_pattern(twinBuffer B, int start, int end, char *out, int out_size)
{
    int cursor = start;
    int length = 0;

    while (cursor != end && length + 1 < out_size)
    {
        out[length++] = B->buffer[cursor];
        cursor = (cursor + 1) % (2 * BUFFER_SIZE);
    }
    out[length] = '\0';
}

trie createTrieNode()
{
    return NULL;
}

void insert(trie root, char *key, TOKEN_TYPE tokenType)
{
    (void)root;
    (void)key;
    (void)tokenType;
}

TOKEN_TYPE search(trie root, char *key)
{
    (void)root;
    return keyword_lookup(key);
}

void resetLexicalErrorCount(void)
{
    lexicalErrorCount = 0;
}

int getLexicalErrorCount(void)
{
    return lexicalErrorCount;
}

static STATE_INFO transition_from_start(char nextSymbol)
{
    switch (nextSymbol)
    {
        case ';':
            return STATE_INFO_OK(START, true, TK_SEM, 0);
        case ',':
            return STATE_INFO_OK(START, true, TK_COMMA, 0);
        case '.':
            return STATE_INFO_OK(START, true, TK_DOT, 0);
        case '(':
            return STATE_INFO_OK(START, true, TK_OP, 0);
        case ')':
            return STATE_INFO_OK(START, true, TK_CL, 0);
        case '[':
            return STATE_INFO_OK(START, true, TK_SQL, 0);
        case ']':
            return STATE_INFO_OK(START, true, TK_SQR, 0);
        case '*':
            return STATE_INFO_OK(START, true, TK_MUL, 0);
        case '/':
            return STATE_INFO_OK(START, true, TK_DIV, 0);
        case '+':
            return STATE_INFO_OK(START, true, TK_PLUS, 0);
        case '-':
            return STATE_INFO_OK(START, true, TK_MINUS, 0);
        case '~':
            return STATE_INFO_OK(START, true, TK_NOT, 0);
        case '@':
            return STATE_INFO_OK(S13, false, NULL_TOKEN, 0);
        case '!':
            return STATE_INFO_OK(S16, false, NULL_TOKEN, 0);
        case '&':
            return STATE_INFO_OK(S18, false, NULL_TOKEN, 0);
        case '=':
            return STATE_INFO_OK(S21, false, NULL_TOKEN, 0);
        case '%':
            return STATE_INFO_OK(S23, false, NULL_TOKEN, 0);
        case ':':
            return STATE_INFO_OK(START, true, TK_COLON, 0);
        case '<':
            return STATE_INFO_OK(S26, false, NULL_TOKEN, 0);
        case '>':
            return STATE_INFO_OK(S33, false, NULL_TOKEN, 0);
        case '_':
            return STATE_INFO_OK(S37, false, NULL_TOKEN, 0);
        case '#':
            return STATE_INFO_OK(S41, false, NULL_TOKEN, 0);
        case '\t':
        case ' ':
        case '\0':
            return STATE_INFO_OK(START, true, BLANK, 0);
        case '\n':
            return STATE_INFO_OK(START, true, NEWLINE, 0);
        default:
            break;
    }

    if (nextSymbol == EOF)
    {
        return STATE_INFO_OK(START, true, EXIT_TOKEN, 0);
    }
    if (is_digit(nextSymbol))
    {
        return STATE_INFO_OK(S44, false, NULL_TOKEN, 0);
    }
    if (nextSymbol >= 'b' && nextSymbol <= 'd')
    {
        return STATE_INFO_OK(S57, false, NULL_TOKEN, 0);
    }
    if (is_alpha_any(nextSymbol))
    {
        return STATE_INFO_OK(S55, false, NULL_TOKEN, 0);
    }

    return STATE_INFO_OK(INVALID, false, NULL_TOKEN, 0);
}

STATE_INFO getNextState(STATE currentState, char nextSymbol)
{
    switch (currentState)
    {
        case START:
            return transition_from_start(nextSymbol);

        case S13:
            return (nextSymbol == '@') ? STATE_INFO_OK(S14, false, NULL_TOKEN, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 1);
        case S14:
            return (nextSymbol == '@') ? STATE_INFO_OK(START, true, TK_OR, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 1);

        case S16:
            return (nextSymbol == '=') ? STATE_INFO_OK(START, true, TK_NE, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 2);

        case S18:
            return (nextSymbol == '&') ? STATE_INFO_OK(S19, false, NULL_TOKEN, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 3);
        case S19:
            return (nextSymbol == '&') ? STATE_INFO_OK(START, true, TK_AND, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 3);

        case S21:
            return (nextSymbol == '=') ? STATE_INFO_OK(START, true, TK_EQ, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 4);

        case S23:
            return (nextSymbol != '\n' && nextSymbol != '\0')
                       ? STATE_INFO_OK(S23, false, NULL_TOKEN, 0)
                       : STATE_INFO_OK(START, true, TK_COMMENT, 0);

        case S26:
            if (nextSymbol == '-')
            {
                return STATE_INFO_OK(S28, false, NULL_TOKEN, 0);
            }
            if (nextSymbol == '=')
            {
                return STATE_INFO_OK(START, true, TK_LE, 0);
            }
            return STATE_INFO_OK(START, true, TK_LT, 1);

        case S28:
            return (nextSymbol == '-') ? STATE_INFO_OK(S29, false, NULL_TOKEN, 0)
                                       : STATE_INFO_OK(START, true, TK_LT, 2);

        case S29:
            return (nextSymbol == '-') ? STATE_INFO_OK(START, true, TK_ASSIGNOP, 0)
                                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 5);

        case S33:
            return (nextSymbol == '=') ? STATE_INFO_OK(START, true, TK_GE, 0)
                                       : STATE_INFO_OK(START, true, TK_GT, 1);

        case S37:
            return is_alpha_any(nextSymbol) ? STATE_INFO_OK(S38, false, NULL_TOKEN, 0)
                                            : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 6);

        case S38:
            if (is_alpha_any(nextSymbol))
            {
                return STATE_INFO_OK(S38, false, NULL_TOKEN, 0);
            }
            if (is_digit(nextSymbol))
            {
                return STATE_INFO_OK(S40, false, NULL_TOKEN, 0);
            }
            return STATE_INFO_OK(START, true, TK_FUNID, 1);

        case S40:
            return is_digit(nextSymbol) ? STATE_INFO_OK(S40, false, NULL_TOKEN, 0)
                                        : STATE_INFO_OK(START, true, TK_FUNID, 1);

        case S41:
            return is_lower_alpha(nextSymbol) ? STATE_INFO_OK(S42, false, NULL_TOKEN, 0)
                                              : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 7);

        case S42:
            return is_lower_alpha(nextSymbol) ? STATE_INFO_OK(S42, false, NULL_TOKEN, 0)
                                              : STATE_INFO_OK(START, true, TK_RUID, 1);

        case S44:
            if (is_digit(nextSymbol))
            {
                return STATE_INFO_OK(S44, false, NULL_TOKEN, 0);
            }
            if (nextSymbol == '.')
            {
                return STATE_INFO_OK(S46, false, NULL_TOKEN, 0);
            }
            return STATE_INFO_OK(START, true, TK_NUM, 1);

        case S46:
            return is_digit(nextSymbol) ? STATE_INFO_OK(S47, false, NULL_TOKEN, 0)
                                        : STATE_INFO_OK(START, true, TK_ERROR, 1);

        case S47:
            return is_digit(nextSymbol) ? STATE_INFO_OK(S48, false, NULL_TOKEN, 0)
                                        : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 8);

        case S48:
            return (nextSymbol == 'E') ? STATE_INFO_OK(S50, false, NULL_TOKEN, 0)
                                       : STATE_INFO_OK(START, true, TK_RNUM, 1);

        case S50:
            if (nextSymbol == '+' || nextSymbol == '-')
            {
                return STATE_INFO_OK(S51, false, NULL_TOKEN, 0);
            }
            if (is_digit(nextSymbol))
            {
                return STATE_INFO_OK(S52, false, NULL_TOKEN, 0);
            }
            return STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 9);

        case S51:
            return is_digit(nextSymbol) ? STATE_INFO_OK(S52, false, NULL_TOKEN, 0)
                                        : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 10);

        case S52:
            return is_digit(nextSymbol)
                       ? STATE_INFO_OK(START, true, TK_RNUM, 0)
                       : STATE_INFO_ERR(INVALID, false, NULL_TOKEN, 0, 11);

        case S55:
            return is_lower_alpha(nextSymbol) ? STATE_INFO_OK(S55, false, NULL_TOKEN, 0)
                                              : STATE_INFO_OK(START, true, TK_FIELDID, 1);

        case S57:
            if (is_lower_alpha(nextSymbol))
            {
                return STATE_INFO_OK(S55, false, NULL_TOKEN, 0);
            }
            if (nextSymbol >= '2' && nextSymbol <= '7')
            {
                return STATE_INFO_OK(S58, false, NULL_TOKEN, 0);
            }
            return STATE_INFO_OK(START, true, TK_FIELDID, 1);

        case S58:
            if (nextSymbol >= '2' && nextSymbol <= '7')
            {
                return STATE_INFO_OK(S59, false, NULL_TOKEN, 0);
            }
            if (nextSymbol >= 'b' && nextSymbol <= 'd')
            {
                return STATE_INFO_OK(S58, false, NULL_TOKEN, 0);
            }
            return STATE_INFO_OK(START, true, TK_ID, 1);

        case S59:
            return (nextSymbol >= '2' && nextSymbol <= '7')
                       ? STATE_INFO_OK(S59, false, NULL_TOKEN, 0)
                       : STATE_INFO_OK(START, true, TK_ID, 1);

        default:
            return STATE_INFO_OK(INVALID, false, NULL_TOKEN, 0);
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
            continue;
        }

        fputc(ch, out);
        ch = fgetc(in);
    }

    fclose(in);
    fclose(out);
}

void initializeLookupTable()
{
    /* Keyword table is static and searched directly via binary search. */
}

char *getTokenName(TOKEN_TYPE type)
{
    for (size_t idx = 0; idx < sizeof(k_token_names) / sizeof(k_token_names[0]); ++idx)
    {
        if (k_token_names[idx].type == type)
        {
            return (char *)k_token_names[idx].name;
        }
    }
    return "INVALID";
}

void printbuffer(twinBuffer B)
{
    for (int idx = 0; idx < 2 * BUFFER_SIZE; ++idx)
    {
        printf("%c ", B->buffer[idx]);
    }
    printf("\n");
}

void populate_buffer(twinBuffer B, FILE *fp)
{
    if (B->index >= BUFFER_SIZE)
    {
        fill_buffer_half(B, fp, 0, BUFFER_SIZE);
    }
    else
    {
        fill_buffer_half(B, fp, BUFFER_SIZE, 2 * BUFFER_SIZE);
    }
}

void handle_comments(twinBuffer B, FILE *fp)
{
    while (B->buffer[B->index] != '\n' && B->buffer[B->index] != '\0')
    {
        advance_with_refill(B, fp);
    }

    advance_with_refill(B, fp);
}

void handle_invalid_error(STATE_INFO state, twinBuffer B, int start, int end)
{
    lexicalErrorCount++;

    char pattern[2 * BUFFER_SIZE + 1];
    memset(pattern, 0, sizeof(pattern));

    if (start == end)
    {
        pattern[0] = B->buffer[start];
        pattern[1] = '\0';
        printf("Line No %d : Error: Unknown Symbol <%s>\n", B->line, pattern);
        B->index = (end + 1) % (2 * BUFFER_SIZE);
        return;
    }

    collect_pattern(B, start, end, pattern, (int)sizeof(pattern));
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
        printf("Line no: %d : Error: Unknown pattern <%s>\n", token->line, token->lexeme);
        return true;
    }

    if (token->type == TK_ID && token->lexemeSize > 20)
    {
        lexicalErrorCount++;
        printf("Line No %d: Error :Variable Identifier is longer than the prescribed length of 20 characters.\n",
               token->line);
        free(token->lexeme);
        free(token);
        return false;
    }

    if (token->type == TK_FUNID && token->lexemeSize > 30)
    {
        lexicalErrorCount++;
        printf("Line No %d: Error :Function Identifier is longer than the prescribed length of 30 characters.\n",
               token->line);
        free(token->lexeme);
        free(token);
        return false;
    }

    return true;
}

tokenInfo getNextToken(twinBuffer B)
{
    if (B->buffer[B->index] == '%')
    {
        handle_comments(B, activeSourceFile);

        tokenInfo comment_token = (tokenInfo)malloc(sizeof(TOKEN));
        if (comment_token == NULL)
        {
            return NULL;
        }

        comment_token->lexeme = (char *)malloc(2);
        if (comment_token->lexeme == NULL)
        {
            free(comment_token);
            return NULL;
        }

        comment_token->lexeme[0] = '%';
        comment_token->lexeme[1] = '\0';
        comment_token->lexemeSize = 1;
        comment_token->line = B->line;
        comment_token->type = TK_COMMENT;
        return comment_token;
    }

    int start = B->index;
    int end = B->index;
    STATE_INFO next_state = getNextState(START, B->buffer[start]);

    while (!next_state.isReturningToken && next_state.nextSTATE != INVALID)
    {
        end = (end + 1) % (2 * BUFFER_SIZE);
        next_state = getNextState(next_state.nextSTATE, B->buffer[end]);
    }

    if (next_state.nextSTATE == INVALID)
    {
        handle_invalid_error(next_state, B, start, end);
        return NULL;
    }

    if (next_state.tokenType == BLANK)
    {
        B->index = (end + 1) % (2 * BUFFER_SIZE);
        return NULL;
    }

    if (next_state.tokenType == NEWLINE)
    {
        B->index = (end + 1) % (2 * BUFFER_SIZE);
        B->line++;
        return NULL;
    }

    int token_end = (end - next_state.redaction + 2 * BUFFER_SIZE) % (2 * BUFFER_SIZE);
    int size = (token_end >= start) ? (token_end - start + 1)
                                     : (2 * BUFFER_SIZE - start + token_end + 1);

    char *lexeme = (char *)malloc((size_t)size + 1);
    if (lexeme == NULL)
    {
        return NULL;
    }

    int cursor = start;
    for (int out = 0; out < size; ++out)
    {
        lexeme[out] = B->buffer[cursor];
        cursor = (cursor + 1) % (2 * BUFFER_SIZE);
    }
    lexeme[size] = '\0';

    tokenInfo token = (tokenInfo)malloc(sizeof(TOKEN));
    if (token == NULL)
    {
        free(lexeme);
        return NULL;
    }

    token->lexeme = lexeme;
    token->lexemeSize = size;
    token->line = B->line;

    if (next_state.tokenType == TK_FIELDID)
    {
        token->type = keyword_lookup(lexeme);
    }
    else if (next_state.tokenType == TK_FUNID)
    {
        token->type = stringcmp(lexeme, "_main") ? TK_MAIN : TK_FUNID;
    }
    else
    {
        token->type = next_state.tokenType;
    }

    B->index = cursor;
    return token;
}

FILE *getStream(FILE *fp)
{
    resetLexicalErrorCount();
    activeSourceFile = fp;

    twinBuffer B = (twinBuffer)malloc(sizeof(TWIN_BUFFER));
    if (B == NULL)
    {
        return fp;
    }

    memset(B->buffer, '\0', sizeof(B->buffer));
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
                {
                    printf("%-12d%-30s%-20s\n", token->line, token->lexeme, getTokenName(token->type));
                }
            }

            if (token->type == TK_COMMENT)
            {
                continue;
            }
        }

        if (crossed_buffer_half(before, B->index))
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
    int can_return = 0;

    if (token != NULL)
    {
        if (token->type == NEWLINE || token->type == TK_COMMENT)
        {
            B->line++;
        }

        if (token->type == TK_COMMENT)
        {
            if (B->buffer[B->index] != '\0')
            {
                return nextToken(B, fp);
            }

            tokenInfo dollar = (tokenInfo)malloc(sizeof(TOKEN));
            if (dollar != NULL)
            {
                dollar->type = DOLLAR;
            }
            return dollar;
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
                can_return = 1;
            }
        }
    }

    if (crossed_buffer_half(before, B->index))
    {
        populate_buffer(B, fp);
    }

    if (can_return)
    {
        return token;
    }

    if (B->buffer[B->index] != '\0')
    {
        return nextToken(B, fp);
    }

    tokenInfo dollar = (tokenInfo)malloc(sizeof(TOKEN));
    if (dollar != NULL)
    {
        dollar->type = DOLLAR;
    }
    return dollar;
}

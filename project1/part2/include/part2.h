#ifndef PART2_H
#define PART2_H

#define MAX_CL_CHAR 512
#define MAX_CMD 4
#define MAX_ARG 16
#define MAX_TOKEN MAX_CL_CHAR
#define MAX_TOKEN_LEN 32

typedef enum {
    False,
    True
} bool;

typedef enum {
    NO_ERROR = 0,
    PARSE_ERROR = 1,
    LAUNCH_ERROR = 2
} Status;

/*        Lexer        */
typedef enum {
    NO_TYPE,
    ID,
    PIPE,
    AMPERSAND,
    REDIR_OUT,
    REDIR_IN
} TokenType;

typedef struct {
    char *value;
    TokenType type;
    int loc;  //location
} Token;

typedef struct {
    Token *tokens[MAX_TOKEN + 1];
    int length;
} TokenList;

/*        Parser        */
typedef enum {
    STATE_START,
    STATE_CMDBLK,
    STATE_AMP_END,
    STATE_REDIR_FILE,
    STATE_REDIR_END,
    STATE_INVALID
} ParserState;

// built in command won't be mixed with pipes
// redirection

typedef struct {
    char *arg[MAX_ARG + 1];
    int proStatus;
    bool isBuiltIn;
    int redirection; // -1: read from file, 0: no redirection, 1: write to file
    char *filePath;
} CommandBlock;

typedef struct CommandLine {
    CommandBlock *commands[MAX_CMD];
    bool background;
    int cmdBlkNum;
} CommandLine;

// utility functions
bool isRedirect(TokenType token);
bool cmdIsBuiltIn(const char *cmd);
bool isDelimiter(TokenType token);
const char *getTokenType(TokenType type);
void printExitStatus(const char cmdLine[MAX_CL_CHAR + 1], const CommandLine *parsedCmdLine);

#endif //PART2_H
#include "stdio.h"
#include "libc.h"
#include "part2.h"
/*
ttwagShell

Issue: 
- Not organized
- Cannot expand any path (EX: ~)
- Cannot Pipe
- Should report syscall error
- How to report failure of library functions
*/

void display_prompt(void) {
    /* Print prompt immediately*/
    printf("ttwagShell@ucd$ ");
    fflush(stdout);
}

/*
Input: an empty string
Output: a string filled with command line inputs
*/
int getCmdLine(char cmdLine[MAX_CL_CHAR + 1]) {
    /* Get command line */
    if (!fgets(cmdLine, MAX_CL_CHAR + 1, stdin)) {
        perror("fgets");
        return 1;
    }
    return 0;
}

/*
- resets cmdLine to an array of 0s
- resets lexedCmdLine to an array of NULL
- resets all of parsedCmdLine->commands[i] to NULL
*/
void resetCmdLine(char cmdLine[MAX_CL_CHAR + 1], TokenList *lexedCmdLine, CommandLine *parsedCmdLine) {
    // reset the command line input buffer
    memset(cmdLine, 0, MAX_CL_CHAR + 1);

    // reset the input buffer to store lexed tokens
    if (lexedCmdLine != NULL) {
        for (int i = 0; i < MAX_TOKEN + 1; i++) {
            if (lexedCmdLine->tokens[i] != NULL) {
                // free each Token
                if (lexedCmdLine->tokens[i]->value != NULL) {
                    free(lexedCmdLine->tokens[i]->value);
                    lexedCmdLine->tokens[i]->value = NULL;
                }
                free(lexedCmdLine->tokens[i]);
                lexedCmdLine->tokens[i] = NULL;
            }
        }
    }

    // reset the parsed command line
    if (parsedCmdLine != NULL) {
        // Check each commands in CommandLine
        for (int i = 0; i < MAX_CMD + 1; i++) {
            // if a Command exist
            if (parsedCmdLine->commands[i] != NULL) {
                // free the arg array
                for (int j = 0; j < MAX_ARG + 1; j++) {
                    if (parsedCmdLine->commands[i]->arg[j] != NULL) {
                        free(parsedCmdLine->commands[i]->arg[j]);
                        parsedCmdLine->commands[i]->arg[j] = NULL;
                    }
                }
                // free file path
                if (parsedCmdLine->commands[i]->filePath != NULL) {
                    free(parsedCmdLine->commands[i]->filePath);
                    parsedCmdLine->commands[i]->filePath = NULL;
                }
                // free each Command *commands
                free(parsedCmdLine->commands[i]);
                parsedCmdLine->commands[i] = NULL;
            }
        }
    }
}

/*
Input: A Token's value, type, and location
Output: Returns a newly allocated Token with the provided inputs
*/
Token *createToken(char *value, TokenType type, int loc) {
    Token *currToken = (Token *)calloc(1, sizeof(Token));
    currToken->value = strdup(value);
    currToken->type = type;
    currToken->loc = loc;
    return currToken;
}

/*
Input: Command line user input (EX: ... \n \0)
Output: An array of tokens without the WHITESPACE;
        lineLen - the size of the array of tokens;

Note: Need to error handle strdup
*/
int lexer(char cmdLine[MAX_CL_CHAR + 1], TokenList *lexedCmdLine) {
    size_t strLen = strlen(cmdLine);
    // If input is within limit, the last char should be a newline
    // Remove the trailing newline char from cmdLine
    if (strLen > 0) {
        if (cmdLine[strLen - 1] == '\n' && strLen == 1) return 1;
        else if (cmdLine[strLen - 1] == '\n') {
            cmdLine[strLen - 1] = '\0';
        }
        else {
            fprintf(stderr, "LEX ERROR: Command Line Input is Too Long\n");
            fprintf(stderr, "Maximum Command Line Input Length (Include \\n): %d\n", MAX_CL_CHAR);
            return 1;
        }
    }
    else {
        fprintf(stderr, "LEX ERROR: Command is Too Short\n");
        return 1;
    }
    
    // Lex the command line
    char tokenBuffer[MAX_TOKEN_LEN + 1] = {};
    int lexInd = 0;
    int cmdInd = 0;
    int buffInd = 0;

    char token = cmdLine[cmdInd];

    while (token != '\0') {
        // lexing
        if (token != ' ' && token != '<' && token != '>' && token != '|' && token != '&') {
            // check token length
            if (buffInd >= MAX_TOKEN_LEN) { 
                fprintf(stderr, "LEX ERROR: A Command, Argument, Path, or File Name Is Too Long\n");
                fprintf(stderr, "Maximum Word Length: %d\n", MAX_TOKEN_LEN);
                return 1;
            }
            tokenBuffer[buffInd] = token;
            buffInd++;
        }
        else {
            // end the current string buffer because [' ' | '<' | '>' | '|']
            if (buffInd > 0) {
                tokenBuffer[buffInd] = '\0';
                lexedCmdLine->tokens[lexInd] = createToken(tokenBuffer, ID , cmdInd - buffInd + 1);
                lexInd++;
                buffInd = 0;
            }
            // delimiters
            if (token != ' ') {
                if (token == '|') {
                    lexedCmdLine->tokens[lexInd] = createToken("|", PIPE, cmdInd + 1);
                }
                else if (token == '<') {
                    lexedCmdLine->tokens[lexInd] = createToken("<", REDIR_IN, cmdInd + 1);
                }
                else if (token == '>') {
                    lexedCmdLine->tokens[lexInd] = createToken(">\0", REDIR_OUT, cmdInd + 1);
                }
                else if (token == '&'){
                    lexedCmdLine->tokens[lexInd] = createToken("&\0", AMPERSAND, cmdInd + 1);
                }
                lexInd++;
            }
        }
        cmdInd++;
        token = cmdLine[cmdInd];
    }
    if (buffInd > 0) {
        tokenBuffer[buffInd] = '\0';
        lexedCmdLine->tokens[lexInd] = createToken(tokenBuffer, ID, cmdInd - buffInd + 1);
        lexInd++;
    }
    lexedCmdLine->length = lexInd;
    return 0;
}

bool isRedirect(TokenType token) {
    if (token == REDIR_IN || token == REDIR_OUT) return True;
    return False;
}

/*
Input: Array of tokens
Output: commandLine object

Note: 
- Currently only works with no piping
- Need to expand ~
*/
/*
int parseCmdLine1(char *lexedCmdLine[MAX_TOKEN + 1], int *lineLen, CommandLine *parsedCmdLine) {
    if (*lineLen <= 0) return 1;
    // create a new CommandBlock object
    if (isRedirect(lexedCmdLine[(*lineLen) - 1])) {
        fprintf(stderr, "PARSE ERROR: Missing File\n");
        return 1;
    }
    
    CommandBlock *commandBlk = (CommandBlock *)calloc(1, sizeof(CommandBlock));
    if (!commandBlk) {
        perror("calloc");
        exit(1);
    }
    int argInd = 0;
    parsedCmdLine->commands[0] = commandBlk;
    commandBlk->arg[0] = strdup(lexedCmdLine[0]);
    for (int i = 0; i < size; i++) {
        // error handling
        if (i == 0 && isRedirect(lexedCmdLine[i])) {
            fprintf(stderr, "PARSE ERROR: Missing A Command for Redirection\n");
            return 1;
        }

        // parse tokens into some CommandBlocks
        if (strcmp(lexedCmdLine[i], "<") == 0) {
            commandBlk->redirection = -1;
            commandBlk->filePath = strdup(lexedCmdLine[i + 1]);
            break;
        }
        else if (strcmp(lexedCmdLine[i], ">") == 0) {
            commandBlk->redirection = 1;
            commandBlk->filePath = strdup(lexedCmdLine[i + 1]);
            break;
        }
        else {
            commandBlk->arg[argInd] = strdup(lexedCmdLine[i]);
            argInd++;
        }
    }
    if (argInd - 2 >= MAX_ARG) {
        fprintf(stderr, "PARSE ERROR: Too Many Argument to Command %s\n", commandBlk->arg[0]);
        fprintf(stderr, "Max Number of Argument: %d\n", MAX_ARG);
        return 1;
    }
    // if built in command
    if (strcmp(lexedCmdLine[0], "cd") == 0 || strcmp(lexedCmdLine[0], "exit") == 0) {
        commandBlk->isBuiltIn = 1;
    }
    else commandBlk->isBuiltIn = 0;
    return 0;
}
*/

/*
Input: Check if input is a built in command
Output: return Yes -> 1, No -> 0
*/
bool cmdIsBuiltIn(char *cmd) {
    if (cmd == NULL) return False;
    else if (strcmp(cmd, "cd") == 0 || strcmp(cmd, "exit") == 0) return True;
    else return False;
}

bool isDelimiter(TokenType token) {
    if (token == PIPE || token == REDIR_IN || token == REDIR_OUT || token == AMPERSAND) {
        return True;
    }
    return False;
}

const char *getState(ParserState state) {
    switch (state) {
        case STATE_START : return "STATE_START";
        case STATE_CMDBLK : return "STATE_CMDBLK";
        case STATE_AMP_END : return "STATE_AMP_END";
        case STATE_REDIR_FILE : return "STATE_REDIR_FILE";
        case STATE_REDIR_END : return "STATE_REDIR_END";
        default : return "STATE_INVALID";
    }
}

/*
*/
int parseCmdLine(TokenList *lexedCmdLine, CommandLine *parsedCmdLine) {
    if (lexedCmdLine == NULL || parsedCmdLine == NULL) return 1;
    if (lexedCmdLine->length <= 0) return 1;
    
    // Parser FSM
    ParserState state = STATE_START;
    CommandBlock *commandBlk = NULL;
    int cmdBlkInd = 0;
    int argInd = 0;
    bool hasPipe = False;
    char *value = NULL;
    TokenType token = NO_TYPE;
    int loc = 0;

    for (int i = 0; i < lexedCmdLine->length; i++) {
        value = lexedCmdLine->tokens[i]->value;
        token = lexedCmdLine->tokens[i]->type;
        loc = lexedCmdLine->tokens[i]->loc;
        printf("\nCurrent State: %s\n", getState(state));
        printf("Curr Token, %s\n", getTokenType(token));
        switch(state) {
            case STATE_START : {
                if (isDelimiter(token)) {
                    fprintf(stderr, "PARSE ERROR: Missing A Command at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                else if (token == ID) {
                    commandBlk = (CommandBlock *)calloc(1, sizeof(CommandBlock));
                    parsedCmdLine->commands[cmdBlkInd] = commandBlk;
                    commandBlk->arg[argInd] = strdup(value);
                    commandBlk->isBuiltIn = cmdIsBuiltIn(value);
                    cmdBlkInd++, argInd++;
                    state = STATE_CMDBLK;
                }
                else state = STATE_INVALID;
                break;
            }
            case STATE_CMDBLK : {
                if (argInd >= MAX_ARG) {
                    fprintf(stderr, "PARSE ERROR: Too Many Argument to Command %s at Location: %d\n", commandBlk->arg[0], loc);
                    fprintf(stderr, "Max Number of Argument: %d\n", MAX_ARG);
                    state = STATE_INVALID;
                }
                else if (argInd == 0 && isDelimiter(token)) {
                    fprintf(stderr, "PARSE ERROR: Missing A Command at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                else if (hasPipe == True && token == REDIR_IN) {
                    fprintf(stderr, "PARSE ERROR: Error: Mis-located Input Redirection at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                else if (token == PIPE) {
                    if (i + 1 >= lexedCmdLine->length) {
                        fprintf(stderr, "PARSE ERROR: Missing A Command at Location: %d\n", loc);
                        state = STATE_INVALID;
                    }
                    else {
                        hasPipe = True;
                        commandBlk = (CommandBlock *)calloc(1, sizeof(CommandBlock));
                        parsedCmdLine->commands[cmdBlkInd] = commandBlk;
                        argInd = 0, cmdBlkInd++;
                        state = STATE_CMDBLK;
                    }
                }
                else if (token == ID) {
                    commandBlk->arg[argInd] = strdup(value);
                    if (argInd == 0) commandBlk->isBuiltIn = cmdIsBuiltIn(value);
                    argInd++;
                    state = STATE_CMDBLK;
                }
                else if (isRedirect(token)) {
                    if (i + 1 >= lexedCmdLine->length) {
                        fprintf(stderr, "PARSE ERROR: No Output File at Location: %d\n", loc);
                        state = STATE_INVALID;
                    }
                    else if (strcmp(value, "<") == 0) {
                        commandBlk->redirection = -1;
                        state = STATE_REDIR_FILE;
                    }
                    else {
                        commandBlk->redirection = 1;
                        state = STATE_REDIR_FILE;
                    }
                }
                else if (token == AMPERSAND) {
                    parsedCmdLine->background = True;
                    state = STATE_AMP_END;
                }
                else state = STATE_INVALID;
                break;
            }
            case STATE_AMP_END : {
                fprintf(stderr, "PARSE ERROR: Mis-located Background Sign: %d\n", loc);
                state = STATE_INVALID;
                break;
            }
            case STATE_REDIR_FILE : {
                if (isRedirect(token)) {
                    fprintf(stderr, "PARSE ERROR: Cannot Have Multiple Redirection at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                else if (isDelimiter(token)) {
                    fprintf(stderr, "PARSE ERROR: No Output File at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                else {
                    commandBlk->filePath = strdup(value);
                    state = STATE_REDIR_END;
                }
                break;
            }
            case STATE_REDIR_END : {
                if (token == AMPERSAND) {
                    parsedCmdLine->background = True;
                    state = STATE_AMP_END;
                }
                else if (isRedirect(token)) {
                    fprintf(stderr, "PARSE ERROR: Cannot Have Multiple Redirection at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                else {
                    fprintf(stderr, "PARSE ERROR: Redirection Is End of Input at Location: %d\n", loc);
                    state = STATE_INVALID;
                }
                break;
            }
            case STATE_INVALID : {
                return 1;
                break;
            }
            default : {
                state = STATE_INVALID;
                break;
            }
        }
    }
    commandBlk = NULL;
    if (state == STATE_INVALID) return 1;
    return 0;
}


/*
Input: CommandLine Object
Output: Executes the command line
- exit command will return a status code 2
- with redirection, overwrite the old file or create a new file with u+w+x if file doesn't exist
*/
int executeCmdLine(CommandLine *parsedCmdLine) {
    pid_t pid = 0;
    printf("Output:\n");
    CommandBlock *commandBlk = parsedCmdLine->commands[0];
    if (commandBlk == NULL) return 0;

    // Handle built-in commands
    if (commandBlk->isBuiltIn) {
        if (strcmp(commandBlk->arg[0], "cd") == 0){
            if (chdir(commandBlk->arg[1]) == -1) {
                fprintf(stderr, "Error: cannot cd into directory\n");
                commandBlk->status = 1;
            }
        }
        else if (strcmp(commandBlk->arg[0], "exit") == 0) {
            printf("Bye...\n");
            commandBlk->status = 0;
            return 2;
        }
    }
    else { // spawn children to complete commands
        pid = fork();
        // parent process
        if (pid > 0) {
            // wait for all children to finish
            int childStatus = 0;
            waitpid(-1, &childStatus, 0);
            commandBlk->status = WEXITSTATUS(childStatus);
        }
        // child process
        else if (pid == 0) {
            // file read
            if (commandBlk->redirection == -1) {
                int fd = open(commandBlk->filePath, O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            } // file write
            else if (commandBlk->redirection == 1) {
                int fd = open(commandBlk->filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            // execute command
            if (execvp(commandBlk->arg[0], commandBlk->arg) == -1) {
                perror("execvp");
                exit(1);
            }
            exit(0);
        }
        else {
            perror("fork");
            exit(1);
        }
    }
    return 0;
}

const char *getTokenType(TokenType type) {
    switch(type) {
        case NO_TYPE: return "NO_TYPE";
        case ID: return "ID";
        case PIPE: return "PIPE";
        case AMPERSAND: return "AMPERSAND";
        case REDIR_OUT: return "REDIR_OUT";
        case REDIR_IN: return "REDIR_IN";
        default: return "ERROR";
    }
}

const char *getBool(bool boolean) {
    if (boolean == True) return "True";
    else return "False";
}

void printLexedCmdLine(TokenList *lexedCmdLine) {
    if (lexedCmdLine == NULL) return;
    printf("\nLexed Command Line:\n");

    if (lexedCmdLine != NULL) {
        Token *token = NULL;
        for (int i = 0; i < lexedCmdLine->length; i++) {
            token = lexedCmdLine->tokens[i];
            if (token != NULL) {
                printf("(%s, %s, %d)\n", token->value, getTokenType(token->type), token->loc);
                
            }
        }
    }
    return;
}

void printParsedCmdLine(CommandLine *parsedCmdLine) {
    if (parsedCmdLine == NULL) return;
    printf("\n\nParsed Command Line:\n");
    printf("Run In Background? %s\n", getBool(parsedCmdLine->background));
    if (parsedCmdLine != NULL) {
        CommandBlock *commandBlk = NULL;
        int cmdCount = 1;
        for (int i = 0; i < MAX_CMD + 1; i++) {
            commandBlk = parsedCmdLine->commands[i];
            if (commandBlk != NULL) {
                printf("\nCommand Block %d\n", cmdCount);
                printf("command: %s\n", commandBlk->arg[0]);
                printf("arg:");
                for (int j = 0; j < MAX_ARG + 1; j++) {
                    if (commandBlk->arg[j] != NULL) {
                        printf(" %s", commandBlk->arg[j]);
                    }
                }
                printf("\n");
                printf("status: %d\n", commandBlk->status);
                printf("isBuiltIn: %d\n", commandBlk->isBuiltIn);
                printf("redirection: %d\n", commandBlk->redirection);
                printf("file: %s\n", commandBlk->filePath);
                printf("\n");
                cmdCount++;
            }
        }
    }
    printf("\n");
}

void printExitStatus(char cmdLine[MAX_CL_CHAR + 1], CommandLine *parsedCmdLine) {
    printf("+ completed '%s' ", cmdLine);
    CommandBlock *commandBlk = NULL;
    for (int i = 0; i < MAX_CMD + 1; i++) {
        commandBlk = parsedCmdLine->commands[i];
        if (commandBlk != NULL) {
            printf("[%d]\n", commandBlk->status);
        }
    }
    printf("\n");
}

int main(void) {
    char cmdLine[MAX_CL_CHAR + 1] = {};
    TokenList lexedCmdLine = {};
    CommandLine parsedCmdLine = {};
    
    while (1) {
        display_prompt();
        resetCmdLine(cmdLine, &lexedCmdLine, &parsedCmdLine);
        
        // Get cmdLine input
        if (getCmdLine(cmdLine)) continue;

        // Lex
        if (lexer(cmdLine, &lexedCmdLine)) continue;
        printLexedCmdLine(&lexedCmdLine);
        printf("\n\n");

        // Parse
        if (parseCmdLine(&lexedCmdLine, &parsedCmdLine)) continue;
        printParsedCmdLine(&parsedCmdLine);

        // Execute
        // int status = executeCmdLine(&parsedCmdLine);
        // printExitStatus(cmdLine, &parsedCmdLine);
        // if (status == 2) exit(0);
    }
    return 0;
}
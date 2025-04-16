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
void resetCmdLine(char cmdLine[MAX_CL_CHAR + 1], char *lexedCmdLine[MAX_TOKEN + 1], CommandLine *parsedCmdLine) {
    // reset the command line input buffer
    memset(cmdLine, 0, MAX_CL_CHAR + 1);

    // reset the input buffer to store lexed tokens
    for (int i = 0; i < MAX_TOKEN + 1; i++) {
        if (lexedCmdLine[i] != NULL) {
            free(lexedCmdLine[i]);
            lexedCmdLine[i] = NULL;
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
Input: Command line user input (EX: ... \n \0)
Output: An array of tokens without the WHITESPACE;
        lineLen - the size of the array of tokens;

Note: Need to error handle strdup
*/
int lexCmdLine(char cmdLine[MAX_CL_CHAR + 1], char *lexedCmdLine[MAX_TOKEN + 1], int *lineLen) {
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
    int redirNum = 0;
    char redirOp[2] = {};
    char token = cmdLine[cmdInd];

    while (token != '\0') {
        // lexing
        if (token != ' ' && token != '<' && token != '>' && token != '|') {
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
                lexedCmdLine[lexInd] = strdup(tokenBuffer);
                lexInd++;
                buffInd = 0;
            }
            // for non white space delimiter
            // if (token == '|') {

            // }
            if (token == '<' || token == '>') {
                redirOp[0] = token;
                lexedCmdLine[lexInd] = strdup(redirOp);
                lexInd++; redirNum++;
            }
        }
        cmdInd++;
        token = cmdLine[cmdInd];
    }
    if (buffInd > 0) {
        tokenBuffer[buffInd] = '\0';
        lexedCmdLine[lexInd] = strdup(tokenBuffer);
        lexInd++;
    }
    *lineLen = lexInd;
    
    if (redirNum > 1) {
        fprintf(stderr, "LEX ERROR: At Most One Redirection\n");
        return 1;
    }
    return 0;
}

int isRedirect(char *str) {
    if (str == NULL) return 0;
    else if (strcmp(str, "<") == 0 || strcmp(str, ">") == 0) return 1;
    return 0;
}

/*
Input: Array of tokens
Output: commandLine object

Note: 
- Currently only works with no piping
- Need to expand ~
*/
int parseCmdLine(char *lexedCmdLine[MAX_TOKEN + 1], int *lineLen, CommandLine *parsedCmdLine) {
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
    int size = *lineLen;
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

void printLexedCmdLine(char *lexedCmdLine[MAX_TOKEN + 1]) {
    printf("\nLexed Command Line:\n");
    for (int i = 0; i < MAX_TOKEN + 1; i++) {
        if (lexedCmdLine[i] != NULL) {
            printf("%s ", lexedCmdLine[i]);
        }
        else return;
    }
    return;
}

void printParsedCmdLine(CommandLine *parsedCmdLine) {
    printf("\n\nParsed Command Line:\n");
    if (parsedCmdLine != NULL) {
        CommandBlock *commandBlk = NULL;
        int cmdCount = 1;
        for (int i = 0; i < MAX_CMD + 1; i++) {
            commandBlk = parsedCmdLine->commands[i];
            if (commandBlk != NULL) {
                printf("Command Block %d\n", cmdCount);
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
    char *lexedCmdLine[MAX_TOKEN + 1] = {};
    int lineLen = 0;
    CommandLine parsedCmdLine = {};
    
    while (1) {
        display_prompt();
        resetCmdLine(cmdLine, lexedCmdLine, &parsedCmdLine);
        
        // Get cmdLine input
        if (getCmdLine(cmdLine)) continue;

        // Lex
        if (lexCmdLine(cmdLine, lexedCmdLine, &lineLen)) continue;
        printLexedCmdLine(lexedCmdLine);
        printf("\n\n");

        // Parse
        if (parseCmdLine(lexedCmdLine, &lineLen, &parsedCmdLine)) continue;
        printParsedCmdLine(&parsedCmdLine);

        // Execute
        // int status = executeCmdLine(&parsedCmdLine);
        // printExitStatus(cmdLine, &parsedCmdLine);
        // if (status == 2) exit(0);
    }
    return 0;
}
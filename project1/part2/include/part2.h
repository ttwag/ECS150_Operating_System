#define MAX_CL_CHAR 512
#define MAX_CMD 4
#define MAX_ARG 16
#define MAX_TOKEN MAX_CL_CHAR
#define MAX_TOKEN_LEN 32

typedef struct CommandBlock CommandBlock;
typedef struct CommandLine CommandLine;

// built in command won't be mixed with pipes
// redirection
// -1: read from file
// 0: no redirection
// 1: write to file
struct CommandBlock {
    char *arg[MAX_ARG + 1];
    int status;
    int isBuiltIn;
    int redirection;
    char *filePath;
};
struct CommandLine {
    CommandBlock *commands[MAX_CMD + 1];
};
#include "stdio.h"
#include "libc.h"
/*
A shell



Issue: 
- Not organized
- Cannot expand any path (EX: ~)
- Cannot Pipe
- Should report syscall error
*/
#define CMDLINE_MAX 512
#define MAX_ARG 16

void display_prompt(void) {
    /* Print prompt immediately*/
    printf("ttwagShell@ucd$ ");
    fflush(stdout);
}

int read_command(char *cmdArry[CMDLINE_MAX + 1]) {
    /* Get command line */
    char cmdLine[CMDLINE_MAX + 1] = {};
    if (!fgets(cmdLine, CMDLINE_MAX + 1, stdin)) {
        perror("fgets");
        return 1;
    }

    // Remove the trailing New Line character from cmdLine
    size_t strLen = strlen(cmdLine);
    if (strLen > 0 && cmdLine[strLen - 1] == '\n') {
        cmdLine[strLen - 1] = '\0';
    }
    
    // Parse the command line
    // Buffer will be deallocated from heap after the child process exits
    char currBuffer[CMDLINE_MAX + 1] = {};
    int arryInd = 0;
    int cmdInd = 0;
    int buffInd = 0;
    while (cmdLine[cmdInd] != '\0') {
        if (cmdLine[cmdInd] != ' ') {
            currBuffer[buffInd] = cmdLine[cmdInd];
            buffInd++;
        }
        else {
            // End of a string
            if (buffInd > 0) {
                currBuffer[buffInd] = '\0';
                cmdArry[arryInd] = strdup(currBuffer);
                arryInd++;
                buffInd = 0;
            }
        }
        cmdInd++;
    }
    if (buffInd > 0) {
        currBuffer[buffInd] = '\0';
        cmdArry[arryInd] = strdup(currBuffer);
        arryInd++;
    }


     /* Print command line if stdin is not provided by terminal */
    // if (!isatty(STDIN_FILENO)) {
    //     printf("%s", cmd);
    //     fflush(stdout);
    // }
    return 0;
}

int main(void) {
    pid_t pid = 0;
    // Store the cmd as an array of strings
    char *cmd[CMDLINE_MAX + 1] = {};
    while (1) {
        // Reset cmd
        memset(cmd, 0, CMDLINE_MAX + 1);
        display_prompt();

        // Start again if read failed
        if (read_command(cmd)) {
            continue;
        }
        // Special case for cd - parent executes the command
        if (cmd[0] != NULL && strcmp(cmd[0], "cd") == 0) {
            int chdir_status = 0;
            if (cmd[1] == NULL) {
                chdir_status = chdir(getenv("HOME"));
            }
            else {
                chdir_status = chdir(cmd[1]);
            }
            if (chdir_status) {
                perror("chdir");
            }
            continue;
        }

        pid = fork();
        // parent process
        if (pid > 0) {
            int status;
            waitpid(-1, &status, 0);
            if (WEXITSTATUS(status)) printf("Exited with Status:%d\n", WEXITSTATUS(status));
        } 
        // child process
        else if (pid == 0) {
            if (cmd[0] != NULL) {
                if (execvp(cmd[0], cmd) == -1) {
                    perror("execvp");
                    exit(1);
                }
            }
            // // print cmd for test
            // int i = 0;
            // while (cmd[i][0] != '\0') {
            //     printf("%s\n", cmd[i]);
            //     i++;
            // }
            exit(0);
        }
        else {
            perror("fork");
            exit(1);
        }
    }
    return 0;
}
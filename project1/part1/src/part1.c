#include "stdio.h"
#include "libc.h"

#define CMDLINE_MAX 50

void display_prompt(void) {
    /* Print prompt immediately*/
    printf("sshell@ucd$ ");
    fflush(stdout);
}

int read_command(char cmdArry[CMDLINE_MAX][CMDLINE_MAX]) {
    /* Get command line */
    char cmdLine[CMDLINE_MAX] = {};
    if (!fgets(cmdLine, CMDLINE_MAX, stdin)) {
        perror("fgets");
        return 1;
    }
    // Parse the command line
    char currBuffer[CMDLINE_MAX] = {};
    
    int ArryInd = 0;
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
                strcpy(cmdArry[ArryInd], currBuffer);
                ArryInd++;
                buffInd = 0;
            }
        }
        cmdInd++;
    }
    if (buffInd > 0) {
        strcpy(cmdArry[ArryInd], currBuffer);
        ArryInd++;
    }
    cmdArry[ArryInd][0] = '\0';


     /* Print command line if stdin is not provided by terminal */
    // if (!isatty(STDIN_FILENO)) {
    //     printf("%s", cmd);
    //     fflush(stdout);
    // }
    return 0;
}

int main(void) {
    pid_t pid = 0;
    char cmd[CMDLINE_MAX][CMDLINE_MAX] = {};
    while (1) {
        display_prompt();
        if (read_command(cmd)) {
            continue;
        }

        pid = fork();
        // Parent
        if (pid > 0) {
            int status;
            waitpid(-1, &status, 0);
            // printf("%s" cmd);
        }
        else if (pid == 0) {
            int i = 0;
            while (cmd[i][0] != '\0') {
                printf("%s\n", cmd[i]);
                i++;
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
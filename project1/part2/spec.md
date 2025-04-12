# Specification

## Command Line Input
* Maximum length of a single command line = **512** characters
* A program has a maximum of **16** non-null arguments
* Maximum length of individual tokens never exceeds **32** characters
* Shell can use $PATH to specify the program's location
* Shell expands "~" to the home directory path
* Shell tokenizes command line into strings with whitespace as the delimiters

## Shell Built-In Command
*  exit
    * exit the shell
    * print "Bye..." to stderr
* cd
* Won't intermix with other commands in a pipe

## Error Management
The ttwagShell deals with three types of errors:
* Failure of library functions.
    * The shell terminates its execution right away. You may optionally use perror() to report the cause of the failure.
* Errors during the parsing of the command line.
    * If the user supplies an incorrect command line, the shell should only display an error message on stderr, discard the invalid input and wait for a new input, but it should not die.
* Errors during the launching of the command.
    * perror();

# Design

## Error Types
| Number | Type |
| :- | :- |
| 1 | Display error message and wait for new input (Don't terminate the shell) |
| 2 | Terminate the shell execution and print error with perror() |


## Lexer
* Input: Command line user input
* Output: An array of tokens without the WHITESPACE
* Error:
    | Error | Error Type |
    | :-| :-|
    | Input exceeds the maximum length (512) | 1 |
    | Program argument exceeds the maximum number (16) | 1 |
    | Token exceeds the maximum length (32) | 1 |
    | Incorrect token | 1 |
    | Failure of the library function | 2 |

* Objective: tokenizes the input based on the lexer rule

```antlr
WHITESPACE : (' ')+ -> skip;
END : '\n';
PIPE : '|';
AMPERSAND : '&';
OUTPUT_REDIRECT : '>'; // At most one < or >
INPUT_REDIRECT : '<';
ID : ([a-zA-Z] | [0-9] | '_' | '.' | '/' | '~')+;
```

## Parser
* Input: Array of tokens
* Output: commandLine object
* Error:
    | Error | Error Type |
    | :- | :- |
    | Exceeds max arg | 1 |
    | Missing a command | 1 |
    | No output file | 1 |
    | Mislocated redirection | 1 |

* Objective: 
    * Parses the lexed tokens into an commandLine object
    * Expands any ~
    * Handle redirection by adding the correct syscall

* Syntax:
```antlr
commandLine 
    : END
    | command AMPERSAND? END
    | redirect AMPERSAND? END
    | redirectCommand AMPERSAND? END
    ;
emptyCommand : END;
command : args ((PIPE args)+)? (PIPE redirect)?;
redirectCommand : args (OUTPUT_REDIRECT | INPUT_REDIRECT) ID;
redirect : args (OUTPUT_REDIRECT | INPUT_REDIRECT) ID;
args : ID ID*;
```

## Executor
* Input: commandLine object
* Output: 
    * Execute commands in the commandLine object
    * Add the status code to the command in commandLine object
* Error: 
    | Error | Error Type |
    | :- | :- |
    | Problem with executing command | 1 |
* Objective: 
    * Executes the commandLine object

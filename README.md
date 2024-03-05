# Simple-Shell (Multi-Processing)
## Objectives:
1. Familiarity with system calls in Unix environment.
2. Introduction to processes and multi-processing.
3. Introduction to signal handling in Unix environment.

## It is required to implement a Unix shell program. A shell is simply a program that conveniently allows you to run other programs. The shell supports the following commands:
1. The internal shell command "exit" which terminates the shell
2. A command with no arguments <br>
    Example: ls, cp, rm     
3. A command with arguments <br>
    Example: ls –l
4. A command, with or without arguments, executed in the background using &. <br>
    Example: gedit & <br>
    Details: In this case, your shell must execute the command and return immediately, not blocking until the command finishes. <br>
####	Requirements: Show that the opened process will be nested as a child process to the shell program via opening the task manager found in the operating system. Additionally, you have to write in a log file (basic text file) when a child process is terminated (main application will be interrupted by a SIGCHLD signal). So you have to implement an interrupt handler to handle this interrupt and do the corresponding action to it.
5. Shell builtin commands <br>
    Commands: cd & echo <br>
cd: Cover all the following cases (assume no spaces in path): <br>
            * cd <br>
            * cd ~ <br>
            * cd .. <br>
            * cd absolute_path <br>
            * cd relative_path_to_current_working_directory <br>
echo: Prints the input after evaluating all expressions (assume input to echo must be within double quotations).
6. Expression evaluation <br>
    Commands: export <br>
    Details: Set values to variables and print variables values. No mathematical operations is needed. <br> 
    Export Details: Accept input of two forms, either a string without spaces, or a full string inside double quotations.

### Compiling and Execution
![image](https://github.com/ranimeshehata/Simple-Shell/assets/121239735/6e7f7312-1d5a-4368-adb0-8dda42a59973)


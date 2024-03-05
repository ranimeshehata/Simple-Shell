#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#define MAGENTA "\x1b[35m"
#define YEL "\e[0;33m"
#define reset "\x1b[0m"
#define MAX_COMMAND_LENGTH 1000
#define MAX_ARGUMENTS 100
char command[MAX_COMMAND_LENGTH];
char *arr[MAX_ARGUMENTS];
char *token;
char *value;
int i;
int k;
FILE *file;
int flag = 0;
pid_t id;

// functions prototypes
void setup_environment();
void parse_input();
void shell();
void execute_shell_builtin(char[]);
void execute_command();
void on_child_exit(pid_t);
void on_signal_exit();
void write_to_log_file(pid_t);
void remove_quotes(char *);

int main()
{
    signal(SIGCHLD, on_signal_exit);
    setup_environment();
    shell();
    return 0;
}

void setup_environment()
{
    // Prompt the user for input
    char directory[1000];
    getcwd(directory, sizeof(directory)); // get the name of the working directory
    chdir(directory);
    printf(MAGENTA "SIMPLE-SHELL:" reset YEL "%s >>" reset, directory); // print working directory
    // Read the user's input
    if (!fgets(command, MAX_COMMAND_LENGTH, stdin))
    {
        perror("Failed to read input");
        shell();
    }
    // repeat function if ENTER button is pressed
    if (command[0] == '\n')
        shell();
    // Remove the newline character
    command[strcspn(command, "\n")] = '\0';
    parse_input();
}

void parse_input()
{
    // Parse the input into tokens
    char temp[1000];
    strcpy(temp, command);
    char *input = strtok(temp, " ");
    // Handle export command
    if (strcmp(input, "export") == 0)
    {
        token = command;
        arr[0] = strsep(&token, " ");
        arr[1] = token;
        remove_quotes(arr[1]);
    }
    else
    {
        // if the command is anything but export
        token = strtok(command, " ");
        i = 0;
        while (token != NULL)
        {
            arr[i++] = token;

            token = strtok(NULL, " ");
        }
        arr[i] = NULL;
        // handle background execution
        if (arr[1] != NULL && !strcmp(arr[1], "&"))
            flag = 1;

        for (int j = 0; arr[j] != NULL; j++)
        {
            if (arr[j][0] == '$')
            {
                char temp[50];
                int l = 0;
                for (k = 1; arr[1][k] != '\0'; k++)
                {
                    temp[l++] = arr[1][k];
                }
                temp[l] = '\0';
                char *g = getenv(temp);
                strcpy(temp, g);
                l = 1;
                token = strtok(temp, " ");
                while (token != NULL)
                {
                    arr[l++] = token;
                    token = strtok(NULL, " ");
                }
                arr[l] = NULL;
            }
        }
    }
    if (strcmp(arr[0], "echo") == 0 || strcmp(arr[0], "cd") == 0 || strcmp(arr[0], "export") == 0)
        execute_shell_builtin(arr[0]);
    // exit command
    else if (strcmp(arr[0], "exit") == 0)
    {
        kill(id, SIGKILL); // kill zombie before exit
        file = fopen("/home/ranime/Desktop/Term6/OS/Labs/Lab1/logs.txt", "a");
        fprintf(file, "\n***** SESSION TERMINATED *****\n");
        fclose(file);
        on_signal_exit();
        exit(EXIT_SUCCESS);
    }
    else
        execute_command();
}

void shell()
{
    do
    {
        flag = 0;
        setup_environment();
    } while (true);
}

void execute_shell_builtin(char string[])
{
    switch (string[1])
    {
    case 'd': // case cd
        if (strcmp(string, "cd") == 0)
        {
            // (~) or No argument provided, go to home directory
            if (arr[1] == NULL || strcmp(arr[1], "~") == 0)
            {
                chdir(getenv("HOME"));
            }
            else if (strcmp(arr[1], "..") == 0)
            { // Move back one directory
                chdir("..");
            }
            else if (arr[1][0] == '/')
            { // Absolute path
                chdir(arr[1]);
            }
            else
            { // Relative path
                char current_path[1024];
                getcwd(current_path, sizeof(current_path));
                strcat(current_path, "/");
                strcat(current_path, arr[1]);
                chdir(current_path);
            }
        }
        else
            perror("Failed to execute command");
        break;
    case 'c': // case echo
        if (strcmp(string, "echo") == 0)
        {
            if (arr[1][0] == '$')
            {
                char temp[100];
                int c = 0;
                for (int j = 1; arr[1][j] != '\0'; j++)
                {
                    temp[c++] = arr[1][j];
                }
                temp[c] = '\0';
                arr[1] = getenv(temp);
            }
            remove_quotes(arr[1]);
            for (int j = 1; arr[j] != NULL; j++)
                printf("%s\n", arr[j]);
        }
        else
            perror("Failed to execute command");
        break;
    case 'x': // case export
        if (strcmp(string, "export") == 0)
        {
            token = strtok(arr[1], "=");
            char *name = token;
            token = strtok(NULL, "=");
            value = token;
            setenv(name, value, 1);
        }
        else
            perror("Failed to execute command");
        break;
    default:
        perror("Failed to execute command");
        shell();
    }
}

void execute_command()
{
    pid_t pid = fork(); // child id
    id = pid;
    if (pid < 0) // Error
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // In the child
    {
        if (execvp(arr[0], arr) == -1)
        {
            perror("Command not found!");
            exit(EXIT_FAILURE);
        }
        else
            execvp(arr[0], arr);
    }
    else // In the parent
    {
        if (flag == 0)
            on_child_exit(pid);
    }
}

void on_child_exit(pid_t pid)
{
    int status;
    waitpid(pid, &status, 0);
    write_to_log_file(pid);
}

void on_signal_exit()
{
    int var;
    int status;
    var = waitpid(id, &status, 0);
    if (var >= 0)
        write_to_log_file(id);
}

void write_to_log_file(pid_t pid)
{
    file = fopen("/home/ranime/Desktop/Term6/OS/Labs/Lab1/logs.txt", "a");
    fprintf(file, "Child process (%d) terminated\n", pid);
    fclose(file);
}

void remove_quotes(char *str)
{
    char *dest = str;
    while (*str)
    {
        if (*str != '"')
            *dest++ = *str;
        str++;
    }
    *dest = '\0';
}
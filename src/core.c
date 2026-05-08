/*
 * core.c - prompt, parsing, execution dispatch
 */

#include "shell.h"

/* builtins table */
static const char *builtins[] = {
    "cd", "pwd", "echo", "ls", "cat", "mkdir",
    "rm", "touch", "whoami", "history", "help",
    "clear", "cp", "exit", NULL
};

void print_banner(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════╗\n");
    printf("  ║         mysh v1.0            ║\n");
    printf("  ║   type 'help' to get started ║\n");
    printf("  ╚══════════════════════════════╝\n");
    printf("\n");
}

void print_prompt(void)
{
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "?");

    /* shorten home dir to ~ */
    char *home = getenv("HOME");
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        printf("\033[1;32m%s\033[0m:\033[1;34m~%s\033[0m$ ",
               SHELL_NAME, cwd + strlen(home));
    } else {
        printf("\033[1;32m%s\033[0m:\033[1;34m%s\033[0m$ ",
               SHELL_NAME, cwd);
    }
    fflush(stdout);
}

int parse_input(char *input, char **args)
{
    int count = 0;
    char *token = strtok(input, " \t");
    while (token && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t");
    }
    args[count] = NULL;
    return count;
}

int is_builtin(const char *cmd)
{
    for (int i = 0; builtins[i]; i++)
        if (strcmp(cmd, builtins[i]) == 0)
            return 1;
    return 0;
}

int execute(char **args, int argc)
{
    if (!args[0]) return 1;

    /* exit */
    if (strcmp(args[0], "exit") == 0)
        return 0;

    /* builtins */
    if (strcmp(args[0], "cd")      == 0) return builtin_cd(args, argc);
    if (strcmp(args[0], "pwd")     == 0) return builtin_pwd();
    if (strcmp(args[0], "echo")    == 0) return builtin_echo(args, argc);
    if (strcmp(args[0], "ls")      == 0) return builtin_ls(args, argc);
    if (strcmp(args[0], "cat")     == 0) return builtin_cat(args, argc);
    if (strcmp(args[0], "mkdir")   == 0) return builtin_mkdir(args, argc);
    if (strcmp(args[0], "rm")      == 0) return builtin_rm(args, argc);
    if (strcmp(args[0], "touch")   == 0) return builtin_touch(args, argc);
    if (strcmp(args[0], "whoami")  == 0) return builtin_whoami();
    if (strcmp(args[0], "history") == 0) return builtin_history();
    if (strcmp(args[0], "help")    == 0) return builtin_help();
    if (strcmp(args[0], "clear")   == 0) return builtin_clear();
    if (strcmp(args[0], "cp")      == 0) return builtin_cp(args, argc);

    /* external command via fork/exec */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        execvp(args[0], args);
        fprintf(stderr, "%s: command not found\n", args[0]);
        exit(1);
    }
    int wstatus;
    waitpid(pid, &wstatus, 0);
    return 1;
}

/*
 * shell.h - shared definitions and prototypes
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>

#define SH_INPUT_LEN 1024
#define MAX_ARGS     64
#define MAX_HISTORY  50
#define SHELL_NAME   "mysh"

/* ---- core ---- */
void  print_banner(void);
void  print_prompt(void);
int   parse_input(char *input, char **args);
int   execute(char **args, int argc);

/* ---- builtins ---- */
int   builtin_cd(char **args, int argc);
int   builtin_pwd(void);
int   builtin_echo(char **args, int argc);
int   builtin_ls(char **args, int argc);
int   builtin_cat(char **args, int argc);
int   builtin_mkdir(char **args, int argc);
int   builtin_rm(char **args, int argc);
int   builtin_touch(char **args, int argc);
int   builtin_whoami(void);
int   builtin_history(void);
int   builtin_help(void);
int   builtin_clear(void);
int   builtin_cp(char **args, int argc);

/* ---- piping / redirection ---- */
int   handle_pipe(char *input);
int   handle_redirect(char **args, int argc);

/* ---- history ---- */
void  history_add(const char *cmd);
char *history_get(int index);
int   history_size(void);

/* ---- helpers ---- */
void  print_tree(const char *path, int depth);
int   is_builtin(const char *cmd);

#endif

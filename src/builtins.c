/*
 * builtins.c - implementation of all built-in shell commands
 */

#include "shell.h"

/* ---- cd ---- */
int builtin_cd(char **args, int argc)
{
    if (argc < 2) {
        char *home = getenv("HOME");
        if (home) chdir(home);
        return 1;
    }
    if (chdir(args[1]) != 0)
        perror("cd");
    return 1;
}

/* ---- pwd ---- */
int builtin_pwd(void)
{
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
    return 1;
}

/* ---- echo ---- */
int builtin_echo(char **args, int argc)
{
    for (int i = 1; i < argc; i++) {
        /* basic env variable expansion */
        if (args[i][0] == '$') {
            char *val = getenv(args[i] + 1);
            printf("%s", val ? val : "");
        } else {
            printf("%s", args[i]);
        }
        if (i < argc - 1) printf(" ");
    }
    printf("\n");
    return 1;
}

/* ---- ls helpers ---- */
static void ls_simple(const char *path)
{
    DIR *d = opendir(path);
    if (!d) { perror("ls"); return; }

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        /* color directories blue */
        if (entry->d_type == DT_DIR)
            printf("\033[1;34m%s\033[0m  ", entry->d_name);
        else
            printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(d);
}

static void ls_all(const char *path)
{
    DIR *d = opendir(path);
    if (!d) { perror("ls"); return; }

    struct dirent *entry;
    struct stat st;
    char full[512];

    printf("%-10s %-6s %-12s %s\n", "perms", "size", "modified", "name");
    printf("%-10s %-6s %-12s %s\n", "----------", "------", "------------", "----");

    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);
        stat(full, &st);

        char tmbuf[13];
        struct tm *tm = localtime(&st.st_mtime);
        strftime(tmbuf, sizeof(tmbuf), "%b %d %H:%M", tm);

        char perms[11];
        snprintf(perms, sizeof(perms), "%c%c%c%c%c%c%c%c%c%c",
            S_ISDIR(st.st_mode)  ? 'd' : '-',
            st.st_mode & S_IRUSR ? 'r' : '-',
            st.st_mode & S_IWUSR ? 'w' : '-',
            st.st_mode & S_IXUSR ? 'x' : '-',
            st.st_mode & S_IRGRP ? 'r' : '-',
            st.st_mode & S_IWGRP ? 'w' : '-',
            st.st_mode & S_IXGRP ? 'x' : '-',
            st.st_mode & S_IROTH ? 'r' : '-',
            st.st_mode & S_IWOTH ? 'w' : '-',
            st.st_mode & S_IXOTH ? 'x' : '-');

        if (S_ISDIR(st.st_mode))
            printf("%-10s %-6s %-12s \033[1;34m%s\033[0m\n",
                   perms, "-", tmbuf, entry->d_name);
        else
            printf("%-10s %-6ld %-12s %s\n",
                   perms, (long)st.st_size, tmbuf, entry->d_name);
    }
    closedir(d);
}

void print_tree(const char *path, int depth)
{
    DIR *d = opendir(path);
    if (!d) return;

    struct dirent *entry;
    char full[512];

    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        for (int i = 0; i < depth; i++) printf("│   ");
        printf("├── ");
        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);
        if (entry->d_type == DT_DIR) {
            printf("\033[1;34m%s\033[0m\n", entry->d_name);
            print_tree(full, depth + 1);
        } else {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(d);
}

int builtin_ls(char **args, int argc)
{
    const char *path = ".";
    if (argc >= 3 && args[2]) path = args[2];

    if (argc >= 2) {
        if (strcmp(args[1], "--all") == 0 || strcmp(args[1], "-a") == 0) {
            ls_all(path);
            return 1;
        }
        if (strcmp(args[1], "--tree") == 0 || strcmp(args[1], "-t") == 0) {
            printf("%s\n", path);
            print_tree(path, 0);
            return 1;
        }
        /* treat as path if not a flag */
        if (args[1][0] != '-') path = args[1];
    }

    ls_simple(path);
    return 1;
}

/* ---- cat ---- */
int builtin_cat(char **args, int argc)
{
    if (argc < 2) {
        fprintf(stderr, "cat: missing file operand\n");
        return 1;
    }

    /* write mode: cat > file */
    if (argc == 3 && strcmp(args[1], ">") == 0) {
        FILE *f = fopen(args[2], "w");
        if (!f) { perror("cat"); return 1; }
        char line[SH_INPUT_LEN];
        printf("(enter text, Ctrl+D to save)\n");
        while (fgets(line, sizeof(line), stdin))
            fputs(line, f);
        fclose(f);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(args[i], "r");
        if (!f) { perror(args[i]); continue; }
        char ch;
        while ((ch = fgetc(f)) != EOF)
            putchar(ch);
        fclose(f);
    }
    return 1;
}

/* ---- mkdir ---- */
int builtin_mkdir(char **args, int argc)
{
    if (argc < 2) { fprintf(stderr, "mkdir: missing operand\n"); return 1; }
    for (int i = 1; i < argc; i++)
        if (mkdir(args[i], 0755) != 0) perror(args[i]);
    return 1;
}

/* ---- rm ---- */
int builtin_rm(char **args, int argc)
{
    if (argc < 2) { fprintf(stderr, "rm: missing operand\n"); return 1; }
    for (int i = 1; i < argc; i++)
        if (remove(args[i]) != 0) perror(args[i]);
    return 1;
}

/* ---- touch ---- */
int builtin_touch(char **args, int argc)
{
    if (argc < 2) { fprintf(stderr, "touch: missing operand\n"); return 1; }
    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(args[i], "a");
        if (!f) perror(args[i]);
        else fclose(f);
    }
    return 1;
}

/* ---- whoami ---- */
int builtin_whoami(void)
{
    struct passwd *pw = getpwuid(getuid());
    printf("%s\n", pw ? pw->pw_name : "unknown");
    return 1;
}

/* ---- history ---- */
int builtin_history(void)
{
    int n = history_size();
    for (int i = 0; i < n; i++)
        printf("  %3d  %s\n", i + 1, history_get(i));
    return 1;
}

/* ---- help ---- */
int builtin_help(void)
{
    printf("\n  Built-in commands:\n\n");
    printf("  %-12s %s\n", "cd [dir]",     "change directory");
    printf("  %-12s %s\n", "pwd",          "print current directory");
    printf("  %-12s %s\n", "ls",           "list files");
    printf("  %-12s %s\n", "ls --all",     "detailed listing");
    printf("  %-12s %s\n", "ls --tree",    "tree view");
    printf("  %-12s %s\n", "cat <file>",   "print file contents");
    printf("  %-12s %s\n", "cat > <file>", "write to file");
    printf("  %-12s %s\n", "mkdir <dir>",  "create directory");
    printf("  %-12s %s\n", "rm <file>",    "remove file or dir");
    printf("  %-12s %s\n", "touch <file>", "create empty file");
    printf("  %-12s %s\n", "cp <s> <d>",   "copy file");
    printf("  %-12s %s\n", "echo [args]",  "print text");
    printf("  %-12s %s\n", "whoami",       "print current user");
    printf("  %-12s %s\n", "history",      "show command history");
    printf("  %-12s %s\n", "clear",        "clear the screen");
    printf("  %-12s %s\n", "exit",         "quit the shell");
    printf("\n  External commands are also supported via fork/exec.\n\n");
    return 1;
}

/* ---- clear ---- */
int builtin_clear(void)
{
    printf("\033[2J\033[H");
    return 1;
}

/* ---- cp ---- */
int builtin_cp(char **args, int argc)
{
    if (argc < 3) { fprintf(stderr, "cp: missing operand\n"); return 1; }

    FILE *src = fopen(args[1], "rb");
    if (!src) { perror(args[1]); return 1; }

    FILE *dst = fopen(args[2], "wb");
    if (!dst) { perror(args[2]); fclose(src); return 1; }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0)
        fwrite(buf, 1, n, dst);

    fclose(src);
    fclose(dst);
    return 1;
}

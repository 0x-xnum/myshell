/*
 * shell.c - main entry point and REPL loop
 */

#include "shell.h"

int main(void)
{
    char input[SH_INPUT_LEN];
    char *args[MAX_ARGS];
    int status = 1;

    print_banner();

    while (status) {
        print_prompt();

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }

        /* strip trailing newline */
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        history_add(input);

        int argc = parse_input(input, args);
        if (argc == 0)
            continue;

        status = execute(args, argc);
    }

    printf("Goodbye!\n");
    return 0;
}

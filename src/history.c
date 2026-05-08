/*
 * history.c - command history ring buffer
 */

#include "shell.h"

static char history[MAX_HISTORY][SH_INPUT_LEN];
static int  hist_count = 0;

void history_add(const char *cmd)
{
    if (hist_count < MAX_HISTORY) {
        strncpy(history[hist_count++], cmd, SH_INPUT_LEN - 1);
    } else {
        /* shift everything up, drop oldest */
        for (int i = 0; i < MAX_HISTORY - 1; i++)
            strncpy(history[i], history[i + 1], SH_INPUT_LEN - 1);
        strncpy(history[MAX_HISTORY - 1], cmd, SH_INPUT_LEN - 1);
    }
}

char *history_get(int index)
{
    if (index < 0 || index >= hist_count) return NULL;
    return history[index];
}

int history_size(void)
{
    return hist_count;
}

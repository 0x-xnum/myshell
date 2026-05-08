# mysh — A Unix Shell in C

OS lab project. Custom shell written in C that handles builtins, external commands via `fork`/`exec`, command history, and a colored prompt.

---

## Features

- **Built-in commands** — `cd`, `pwd`, `ls`, `cat`, `mkdir`, `rm`, `touch`, `cp`, `echo`, `whoami`, `clear`, `history`, `help`
- **External commands** — anything not built-in runs via `fork` + `execvp`
- **ls variants** — plain, `--all` (like `ls -la`), `--tree` (recursive tree view)
- **cat write mode** — `cat > file.txt` lets you type content directly
- **Command history** — tracks last 50 commands, view with `history`
- **Colored prompt** — shows current directory, home shown as `~`
- **Env variable expansion** — `echo $HOME` works

---

## Build

```bash
make
./mysh
```

Requires GCC and a Linux system. Tested on Ubuntu 22.04.

---

## Usage

```
mysh:~$ help          # list all commands
mysh:~$ ls --tree     # recursive tree
mysh:~$ cat > notes.txt   # write a file
mysh:~$ history       # see past commands
mysh:~$ exit
```

---

## Project Structure

```
.
├── include/
│   └── shell.h        # all structs, defines, prototypes
├── src/
│   ├── shell.c        # main(), REPL loop
│   ├── core.c         # prompt, parser, execute dispatcher
│   ├── builtins.c     # all built-in command implementations
│   └── history.c      # ring buffer for command history
├── docs/
│   └── commands.md    # command reference
├── Makefile
└── README.md
```

---

## How it works

The shell runs a simple read-eval-print loop:

1. Print prompt with current directory
2. Read a line of input
3. Tokenize it into args
4. If it's a builtin → call directly; otherwise `fork()` + `execvp()`
5. `waitpid()` on the child, repeat

The prompt shortens the home directory to `~` and colors the shell name green and the path blue using ANSI codes.

---

## Commands

| Command | Description |
|---------|-------------|
| `cd [dir]` | Change directory (no arg goes home) |
| `pwd` | Print working directory |
| `ls` | List files, dirs in blue |
| `ls --all` | Detailed view with perms/size/date |
| `ls --tree` | Recursive tree |
| `cat <file>` | Print file |
| `cat > <file>` | Write to file interactively |
| `mkdir <dir>` | Create directory (multi-arg) |
| `rm <file>` | Remove file |
| `touch <file>` | Create empty file |
| `cp <src> <dst>` | Copy file |
| `echo [args]` | Print text, expands `$VAR` |
| `whoami` | Current user |
| `history` | Last 50 commands |
| `clear` | Clear screen |
| `help` | Show this list |
| `exit` | Quit |

---

## License

MIT

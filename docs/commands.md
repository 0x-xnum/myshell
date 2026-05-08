# Command Reference

Quick reference for all commands supported by mysh.

## Navigation

**cd** — change directory
```
cd           # go to home
cd docs      # go into docs/
cd ..        # go up one level
cd /tmp      # absolute path
```

**pwd** — print current path
```
pwd
```

## File Operations

**ls** — list directory contents
```
ls                # simple listing, dirs colored blue
ls --all          # shows permissions, size, modification time
ls --tree         # recursive tree view of current dir
ls /some/path     # list a specific directory
```

**cat** — read or write files
```
cat file.txt          # print file to stdout
cat a.txt b.txt       # print multiple files
cat > output.txt      # write mode (Ctrl+D to finish)
```

**touch** — create empty file(s)
```
touch notes.txt
touch a.txt b.txt c.txt
```

**mkdir** — create directory/directories
```
mkdir src
mkdir src include tests
```

**rm** — remove file or empty directory
```
rm file.txt
rm dir/
```

**cp** — copy a file
```
cp source.txt dest.txt
```

## Output

**echo** — print text, supports `$VAR` expansion
```
echo hello world
echo $HOME
echo $USER
```

## System Info

**whoami** — print current username

**history** — print last 50 commands with index numbers

## Shell

**help** — list all commands

**clear** — clear the terminal screen

**exit** — quit the shell

## External Commands

Any command not listed above is passed to the OS via `execvp`. So things like `gcc`, `python3`, `git`, etc. all work normally.

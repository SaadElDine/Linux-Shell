# Linus Mini Shell Project - Operating Systems Lab

## Introduction

This project involves building a mini shell to execute simple shell commands. Lex and Yacc are used for building the scanner and parser, eliminating the need to implement a parser from scratch.

### First Part: Implement Lex and Yacc

#### Running the Shell

- Build the shell: `make`
- Run the shell: `./shell`
- Enter commands like:
  - `ls -al`
  - `ls -al aaa bbb > out`

## Second Part: Process Creation, Execution, File Redirection, Pipes, and Background

1. Execute simple commands:
   - Use `fork()` and `execvp()` for each simple command.
   - If not in the background, wait for the last simple command using `waitpid()`.

2. Implement file redirection:
   - Use `dup2()` to redirect file descriptors 0, 1, or 2 to new files.
   - Execute commands like:
     - `ls -al > out`
     - `ls -al >> out`
     - `cat out`
     - `cat < out`
     - `cat < out > out2`
     - `cat out2`
     - `ls /tt >>& out2`

3. Implement pipes:
   - Use `pipe()` to create pipes.
   - Use `dup2()` for redirection.
   - Execute commands like:
     - `ls -al | grep command`
     - `ls -al | grep command | grep command.o`

## Third Part: Control-C, Exit, Change Directory, Process Creation Log File

1. Ignore ctrl-c (SIGINT).
2. Implement an internal `exit` command to exit the shell.
3. Implement `cd [dir]` command to change the current directory.
4. Extend Lex to support any character in arguments.
5. Create a log file with child termination logs using SIGCHLD signal.

### Final Part:

1. Implement wildcarding:
   - "*" matches 0 or more nonspace characters.
   - "?" matches one nonspace character.
   - Example wildcard commands:
     - `echo *`
     - `echo *.cc`
     - `echo c*.cc`
     - `echo M*f*`
     - `echo /tmp/*`
     - `echo /*t*/*`
     - `echo /dev/*`

## Conclusion

The mini shell project successfully implements a basic shell with features like process execution, file redirection, pipes, and internal commands. It demonstrates the application of Lex and Yacc for parsing shell commands.

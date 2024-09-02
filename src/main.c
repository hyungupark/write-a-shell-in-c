#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Shell Builtins
 *
 * Most commands a shell executes are programs, but not all of them. Some of them are
 * built right into the shell.
 *
 * The reason is actually pretty simple. If you want to change directory, you need to
 * use the function "chdir()". The thing is, the current directory is a property of
 * a process. So, if you wrote a program called "cd" that changed directory, it would
 * just change its own current directory, and then terminate. Its parent process's
 * current directory would be unchanged. Instead, the shell process itself needs to
 * execute "chdir()", so that its own current directory is updated. Then, when it
 * launches child processes, they will inherit that directory too.
 *
 * Similarly, if there was a program named "exit", it wouldn't be able to exit the shell
 * that called it. That command also needs to be built into the shell. Also, most shells
 * are configured by running configuration scripts, like "~/.bashrc". Those scripts use
 * commands that changed the operation of the shell. These commands could only change
 * the shell's operation if they were implemented within the shell process itself.
 *
 * So, it makes sense that we need to add some commands to the shell itself. The ones I
 * added to my shell are "cd", "exit", and "help".
 */

/*
 * Function declarations for builtin shell commands:
 *   - cd: sh_cd
 *   - help: sh_help
 *   - exit: sh_exit
 */

int sh_cd(char **args);

int sh_help(char **args);

int sh_exit(char **args);


/*
 * List of builtin commands, followed by their corresponding functions.
 */

char *builtin_str[] = {
        "cd",
        "help",
        "exit",
};

int (*builtin_func[])(char **) = {
        &sh_cd,
        &sh_help,
        &sh_exit,
};

int sh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


/*
 * Builtin function implementations.
 */

/**
 * @brief Builtin command: change directory.
 * @param args List of args. args[0] is "cd". args[1] is the directory.
 * @return Always returns 1, to continue executing.
 */
int sh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "sh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("sh");
        }
    }
    return 1;
}

/**
 * @brief Builtin command: print help.
 * @param args List of args. Not examined.
 * @return Always returns 1, to continue executing.
 */
int sh_help(char **args) {
    int i;
    printf("SH\m");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < sh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
 * @brief Builtin command: exit.
 * @param args List of args. Not examined.
 * @return Always returns 0, to terminate execution.
 */
int sh_exit(char **args) {
    return 0;
}


/*
 * How shells start processes
 *
 * Staring processes is the main function of shells. So writing a shell means that
 * you need to know exactly what's going on with processes and how they start.
 *
 * There are only two ways of starting processes on Unix. The first one (which almost
 * doesn't count) is by being "Init". When a Unix computer boots, its kernel is loaded.
 * Once it is loaded and initialized, the kernel starts only one process, which is called "Init".
 * This process runs for the entire length of time that the computer is on, and it manages
 * loading up the rest of the processes that you need for your computer to be useful.
 *
 * Since most programs aren't "Init", that leaves only one practical way for processes to
 * get started: the "fork()" system call. When this function is called, the operating system
 * makes a duplicate of the process and starts them both running. The original process is
 * called the "parent", and the new one is called the "child". "fork()" returns 0 to the child
 * process, and it returns to the parent the process ID number (PID) of its child. In essence,
 * this means that the only way for new processes is to start is by an existing one duplicating
 * itself.
 *
 * This might sound like a problem. Typically, when you want to run a new process, you don't
 * just want another copy of the same program - you want to run a different program. THat's
 * what the "exec()" system call is all about. It replaces the current running program with
 * an entirely new one. This means that when you call "exec()", the operating system stops
 * your process, loads up the new program, and starts that one in its place. A process never
 * returns from an "exec()" call (unless there's an error).
 *
 * With these two system calls, we have the building blocks for how most programs are run on Unix.
 * First, an existing process forks itself into two separate ones. Then, the child uses "exec()" to
 * replace itself with a new program. The parent process can continue doing other things, and it can
 * even keep tabs on its children, using the system call "wait()".
 */

/**
 * @brief Launch a program and wait for it to terminate.
 * @param args Null terminated list of arguments (including program).
 * @return Always returns 1, to continue execution.
 */
int sh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("sh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("sh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


/*
 * Parsing the line
 *
 * We are going to parse that line into a list of arguments.
 * We won't allow quoting or backslash escaping in our command line arguments.
 * Instead, we will simply use whitespace to separate arguments from each other.
 * So the command echo "this message" would not call echo with a single argument this message,
 * but rather it would call echo with two arguments: "this" and "message"
 *
 * With those simplifications, all we need to do is "tokenize" the string using whitespace as delimiters.
 * That means we can break out the classic library function "strtok" to do some of the dirty work for us.
 */

#define SH_TOK_BUFFER_SIZE 64
#define SH_TOK_DELIMITER " \t\r\n\a"

/**
 * @brief Split a line into tokens.
 * @param line The line.
 * @return Null-terminated array of tokens.
 */
char **sh_split_line(char *line) {
    int buffer_size = SH_TOK_BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOK_DELIMITER);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size += SH_TOK_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            if (!tokens) {
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SH_TOK_DELIMITER);
    }
    tokens[position] = NULL;
    return tokens;
}


/*
 * Reading a line
 *
 * Reading a line stdin sounds so simple, but in C it can be a hassle.
 * The sad thing is that you don't know ahead of time how much text a user
 * will enter into their shell. You can't simply allocate a block and hope
 * they don't exceed it. Instead, you need to start with a block, and if they
 * do exceed it, reallocate with more space. This is common strategy in C.
 */

#define SH_READ_LINE_BUFFER_SIZE 1024

/**
 * @brief Read a line of input from stdin.
 * @return The line from stdin.
 */
char *sh_read_line(void) {
    int buffer_size = SH_READ_LINE_BUFFER_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if (!buffer) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer. reallocate.
        if (position >= buffer_size) {
            buffer_size += SH_READ_LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "sh: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


/*
 * Basic loop of a shell
 *
 * Shell does the following during its loop:
 *   1. Read: Read the command from standard input.
 *   2. Parse: Separate the command string into a program and arguments.
 *   3. Execute: Run the parsed command.
 */

/**
 * @brief Loop getting input and executing it.
 */
void sh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");

        // Read
        line = sh_read_line();

        // Parse

        // Execute

        free(line);
        free(args);
    } while (status);
}


/*
 * A shell does three main things in its lifetime.
 *   1. Initialize: In this step, a typical shell would read and execute its configuration files.
 *      These change aspects of the shell's behavior.
 *   2. Interpret: Next, the shell reads commands from stdin (which could be interactive, or a file)
 *      and executes them.
 *   3. Terminate: After its commands are executed, the shell executes any shutdown commands,
 *      frees up any memory, and terminates.
 */

/**
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return status code.
 */
int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    sh_loop();

    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}
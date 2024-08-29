#include <stdio.h>
#include <stdlib.h>

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
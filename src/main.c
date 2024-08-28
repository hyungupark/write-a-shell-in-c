#include <stdio.h>
#include <stdlib.h>

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
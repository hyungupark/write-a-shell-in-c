#include <stdlib.h>

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

    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}
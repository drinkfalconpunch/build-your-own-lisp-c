#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
// #include <editline/history.h> // not on mac?

int main(int argc, char **argv)
{
    // Print version and exit information
    puts("Lispy version 0.0.0.0.1");
    puts("Press Ctrl+C to Exit\n");

    while (1)
    {
        // Output prompt
        char *input = readline("lispy> ");

        // add input to history
        add_history(input);

        // Echo input
        printf("No you're a %s", input);

        // free retrieved input
        free(input);
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main(int argc, char** argv) {

    /* print version and exit information */
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to exit\n");

    /* in a never ending loop */
    while (1) {
        // output prompt and get input
        char* input = readline("lispy> ");

        add_history(input);

        // echo input back to user
        printf("No you're a %s\n", input);

        free(input);
    }

    return 0;
}

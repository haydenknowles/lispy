#include "mpc.h"
#include <editline/readline.h>

int main(int argc, char** argv) {

    /* Create Some Parsers */
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    /* Define them with the following Language */
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                   \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
              Number, Operator, Expr, Lispy);
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

    /* Undefine and Delete our Parsers */
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    
    return 0;
}

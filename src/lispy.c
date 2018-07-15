#include <math.h>
#include <editline/readline.h>

#include "mpc.h"

enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
    int type;
    double num;
    int err;
} lval;

 lval eval_op(lval x, char* op, lval y);
 lval eval(mpc_ast_t* t);
 lval lval_num(double x);
 lval lval_err(int x);
 void lval_print(lval v);
 void lval_println(lval v);

int main(int argc, char** argv) {

    // create new parsers
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    // define the language
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                   \
    number   : /-?[0-9]+(\\.[0-9]+)?/ ;                             \
    operator : '+' | '-' | '*' | '/' | '%' | '^' ;      \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
              Number, Operator, Expr, Lispy);

    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+C to exit\n");

    while (1) {
        // output prompt and get input
        char* input = readline("lispy> ");

        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // on successful parse print the AST
            //mpc_ast_print(r.output);
            lval result = eval(r.output);
            lval_println(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    
    return 0;
}

 lval eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        // return atoi(t->contents);
        errno = 0;
        double x = strtod(t->contents, NULL);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    // the operator is always the second child
    char* op = t->children[1]->contents;
    lval x = eval(t->children[2]);

    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    if (i == 3 && strcmp(op, "-") == 0) {
        x = eval_op(lval_num(0L), op, x);
    }

    return x;
}

lval eval_op(lval x, char* op, lval y) {
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }
    
    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "%") == 0) { return lval_num(fmod(x.num, y.num)); }
    if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }
    
    if (strcmp(op, "/") == 0) {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
    }
    return lval_err(LERR_BAD_OP);
}

 lval lval_num(double x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

 lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

 void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM: printf("%.2f", v.num); break;
        case LVAL_ERR:
            if (v.err == LERR_DIV_ZERO) {
                printf("Error: Division by zero!");
            }
            if (v.err == LERR_BAD_OP) {
                printf("Error: Invalid operator!");
            }
            if (v.err == LERR_BAD_NUM) {
                printf("Error: Invalid number!");
            }
            break;
    }
}

 void lval_println(lval v) { lval_print(v); putchar('\n'); }
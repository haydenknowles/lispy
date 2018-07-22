#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include <editline/readline.h>

#include "mpc.h"

enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct lval {
    int type;
    double num;
    char* err;
    char* sym;
    // length of cell list
    int count;
    struct lval** cell;
} lval;

lval eval_op(lval x, char* op, lval y);
lval eval(mpc_ast_t* t);
lval* lval_num(double x);
lval* lval_err(char* m);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
void lval_print(lval* v);
void lval_println(lval* v);
void lval_del(lval* v);
lval* lval_read(mpc_ast_t* t);
void lval_expr_print(lval* v, char open, char close);
lval* lval_add(lval* v, lval* x);
lval* lval_eval(lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* builtin_op(lval* a, char* op);

int main(int argc, char** argv) {

    // create new parsers
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    // define the language
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                               \
    number   : /-?[0-9]+(\\.[0-9]+)?/ ;             \
    symbol   : '+' | '-' | '*' | '/' | '%' | '^' ;  \
    sexpr    : '(' <expr>* ')' ;                    \
    expr     : <number> | <symbol> | <sexpr> ;      \
    lispy    : /^/ <expr>* /$/ ;                    \
    ",
              Number, Symbol, Sexpr, Expr, Lispy);

    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+C to exit\n");

    while (1) {
        // output prompt and get input
        char* input = readline("lispy> ");

        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);

            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);
    
    return 0;
}

 lval* lval_num(double x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

 lval* lval_err(char* m) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

 void lval_print(lval* v) {
    switch (v->type) {
        case LVAL_NUM: printf("%.2f", v->num); break;
        case LVAL_ERR: printf("Error: %s", v->err); break;
        case LVAL_SYM: printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    }
}

 void lval_println(lval* v) { lval_print(v); putchar('\n'); }

 void lval_del(lval* v) {
    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

        case LVAL_SEXPR:
            // loop through the list of lval structs and delete all 
            // elements inside each one
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
     

     free(v);
    }
 }

 lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
 }

 lval* lval_read(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    // if root(>) or sexpr then create empty list
    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0)  { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
 }

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        lval_print(v->cell[i]);

        if (i != v->count-1) {
            putchar(' ');
        }
    }
    putchar(close);
}

lval* lval_eval_sexpr(lval* v) {

    // evaluate children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // error checking
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    // empty expression
    if (v->count == 0) { return v; }

    if (v->count == 1) { lval_take(v, 0); }

    // make sure first element is a symbol
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) { 
        lval_del(f);
        lval_del(v);
        return lval_err("S-expression does not start with symbol!");
    }

    lval* result = builtin_op(v, f->sym);
    lval_del(f);
    return result;
}

lval* lval_eval(lval* v) {
    // evaluate s-expressions
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
    // all other lval types remain the same
    return v;
}

lval* lval_pop(lval* v, int i) {
    lval* x = v->cell[i];
    
    memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));

    v->count--;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = v->cell[i];

    lval_del(v);
    return x;
}

lval* builtin_op(lval* a, char* op) {
    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM){
            lval_del(a);
            return lval_err("Cannot operate on a non-number!");
        }
    }

    lval* x = lval_pop(a, 0);

    if (strcmp(op, "-") == 0 && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        lval* y = lval_pop(a, 0);
        if (strcmp(op, "%") == 0) { x->num = fmod(x->num, y->num); }
        if (strcmp(op, "^") == 0) { x->num = pow(x->num, y->num); }
        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division by zero!");
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}
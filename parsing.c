#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// windows readline function
char* readline(char* prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

// windows fake add_history function
void add_history(char* unused)
{
}

#else
#include <editline/readline.h>
// #include <editline/history.h> // not on mac?
#endif

// use operator string to perform operation
long eval_op(long x, char* op, long y)
{
    if (strcmp(op, "+") == 0) {
        return x + y;
    }
    if (strcmp(op, "-") == 0) {
        return x - y;
    }
    if (strcmp(op, "*") == 0) {
        return x * y;
    }
    if (strcmp(op, "/") == 0) {
        return x / y;
    }
    if (strcmp(op, "%") == 0) {
        return x % y;
    }
    if (strcmp(op, "^") == 0) {
        return pow(x, y);
    }
    return 0;
}

long eval(mpc_ast_t* t)
{
    // if tagged as number, return it
    if (strstr(t->tag, "number")) {
        // printf("Number: %s\n", t->contents);
        return atoi(t->contents);
    }

    // the operator is always second child
    char* op = t->children[1]->contents;
    // printf("Operator: %s\n", op);

    // store third child in 'x'
    long x = eval(t->children[2]);

    // iterate the remaining children and combine
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}

int main(int argc, char** argv)
{
    // create parsers
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    // define the language
    mpca_lang(MPCA_LANG_DEFAULT,
        " \
    number: /-?[0-9]+\\.?[0-9]*/ ; \
    operator: '+' | '-' | '*' | '/' | '%' | '^'; \
    expr: <number> | '(' <operator> <expr>+ ')'; \
    lispy: /^/ <operator> <expr>+ /$/ ; \
    ",
        Number,
        Operator,
        Expr,
        Lispy);

    // Print version and exit information
    puts("Lispy version 0.0.0.0.1");
    puts("Press Ctrl+C to Exit\n");

    while (1) {
        // Output prompt
        char* input = readline("lispy> ");

        // add input to history
        add_history(input);

        // attempt to parse user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // // on success print the AST
            // mpc_ast_print(r.output);

            // // load ast from output
            // mpc_ast_t* a = r.output;
            // printf("Tag: %s\n", a->tag);
            // printf("Contents: %s\n", a->contents);
            // printf("Number of children: %i\n", a->children_num);

            // // get first child
            // mpc_ast_t* c0 = a->children[0];
            // printf("First child tag: %s\n", c0->tag);
            // printf("First child contents: %s\n", c0->contents);
            // printf("First child number of children: %i\n", c0->children_num);

            // mpc_ast_delete(r.output);

            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        } else {
            // otherwise print error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // free retrieved input
        free(input);
    }

    // undefine and clean up parsers
    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}
#ifndef FUNCTION_H
#define FUNCTION_H

#define FUNCTION_MAX 256

typedef struct
{
    char expression[FUNCTION_MAX];
    int valid;
} MathFunction;

void FunctionInit(
    MathFunction *function
);

void FunctionSet(
    MathFunction *function,
    const char *expression
);

double FunctionEvaluate(
    MathFunction *function,
    double x
);

#endif

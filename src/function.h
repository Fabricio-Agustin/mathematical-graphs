#ifndef FUNCTION_H
#define FUNCTION_H

#define FUNCTION_MAX 256

typedef struct
{
    char expresion[FUNCTION_MAX];
    int valida;
} MathFunction;

void FunctionInit(MathFunction *funcion);

void FunctionSet(
    MathFunction *funcion,
    const char *expresion
);

double FunctionEvaluate(
    MathFunction *funcion,
    double x
);

#endif

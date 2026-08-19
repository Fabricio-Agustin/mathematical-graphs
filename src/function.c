#include "function.h"
#include "parser_utils.h" 

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

static const char *texto;
static double valorX;
static int errorParser;

static void SaltarEspacios(void)
{
    while (*texto && isspace((unsigned char)*texto))
        texto++;
}

static double Expresion(void);

static double Numero(void)
{
    char *final;
    double valor;

    SaltarEspacios();

    valor = strtod(texto, &final);

    if (final == texto)
    {
        errorParser = 1;
        return 0.0;
    }

    texto = final;

    return valor;
}

static double Factor(void)
{
    double resultado;

    SaltarEspacios();

    if (*texto == '+')
    {
        texto++;
        return Factor();
    }

    if (*texto == '-')
    {
        texto++;
        return -Factor();
    }

    if (*texto == '(')
    {
        texto++;

        resultado = Expresion();

        SaltarEspacios();

        if (*texto != ')')
        {
            errorParser = 1;
            return 0.0;
        }

        texto++;

        return resultado;
    }

    if (isalpha((unsigned char)*texto))
    {
        char nombre[20];
        int i = 0;

        while (isalpha((unsigned char)*texto) && i < 19)
        {
            nombre[i++] = (char)tolower((unsigned char)*texto);
            texto++;
        }

        nombre[i] = '\0';

        if (strcmp(nombre, "x") == 0)
        {
            return valorX;
        }

        if (strcmp(nombre, "pi") == 0)
        {
            return 3.14159265358979323846;
        }

        if (strcmp(nombre, "e") == 0)
        {
            return 2.71828182845904523536;
        }

        SaltarEspacios();

        if (*texto != '(')
        {
            errorParser = 1;
            return 0.0;
        }

        texto++;

        resultado = Expresion();

        SaltarEspacios();

        if (*texto != ')')
        {
            errorParser = 1;
            return 0.0;
        }

        texto++;

        if (strcmp(nombre, "sin") == 0)
            return sin(resultado);

        if (strcmp(nombre, "cos") == 0)
            return cos(resultado);

        if (strcmp(nombre, "tan") == 0)
            return tan(resultado);

        if (strcmp(nombre, "sqrt") == 0)
        {
            if (resultado < 0)
            {
                errorParser = 1;
                return 0.0;
            }

            return sqrt(resultado);
        }

        if (strcmp(nombre, "log") == 0)
        {
            if (resultado <= 0)
            {
                errorParser = 1;
                return 0.0;
            }

            return log(resultado);
        }

        if (strcmp(nombre, "abs") == 0)
            return fabs(resultado);

        errorParser = 1;
        return 0.0;
    }

    return Numero();
}

static double Potencia(void)
{
    double izquierda;
    double derecha;

    izquierda = Factor();

    SaltarEspacios();

    if (*texto == '^')
    {
        texto++;

        derecha = Potencia();

        izquierda = pow(izquierda, derecha);
    }

    return izquierda;
}

static double Termino(void)
{
    double resultado;
    double valor;

    resultado = Potencia();

    while (!errorParser)
    {
        SaltarEspacios();

        if (*texto == '*')
        {
            texto++;

            valor = Potencia();
            resultado *= valor;
        }
        else if (*texto == '/')
        {
            texto++;

            valor = Potencia();

            if (fabs(valor) < 1e-15)
            {
                errorParser = 1;
                return 0.0;
            }

            resultado /= valor;
        }
        else
        {
            break;
        }
    }

    return resultado;
}

static double Expresion(void)
{
    double resultado;
    double valor;

    resultado = Termino();

    while (!errorParser)
    {
        SaltarEspacios();

        if (*texto == '+')
        {
            texto++;

            valor = Termino();
            resultado += valor;
        }
        else if (*texto == '-')
        {
            texto++;

            valor = Termino();
            resultado -= valor;
        }
        else
        {
            break;
        }
    }

    return resultado;
}

void FunctionInit(MathFunction *funcion)
{
    if (!funcion)
        return;

    funcion->expresion[0] = '\0';
    funcion->valida = 0;
}

void FunctionSet(
    MathFunction *funcion,
    const char *expresion
)
{
    if (!funcion)
        return;

    if (!expresion)
    {
        FunctionInit(funcion);
        return;
    }

    PreprocesarExpresion(expresion, funcion->expresion, FUNCTION_MAX);

    funcion->valida = 1;
}

double FunctionEvaluate(
    MathFunction *funcion,
    double x
)
{
    double resultado;

    if (!funcion || !funcion->valida)
        return 0.0;

    texto = funcion->expresion;
    valorX = x;
    errorParser = 0;

    resultado = Expresion();

    SaltarEspacios();

    if (*texto != '\0')
        errorParser = 1;

    if (errorParser)
        return NAN;

    return resultado;
}

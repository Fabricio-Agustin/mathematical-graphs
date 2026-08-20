#include "function.h"
#include "parser_utils.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

static const char *text;
static double valueX;
static int parserError;

static void SkipSpaces(void)
{
    while (*text && isspace((unsigned char)*text))
        text++;
}

static double Expression(void);

static double Number(void)
{
    char *end;
    double value;

    SkipSpaces();

    value = strtod(text, &end);

    if (end == text)
    {
        parserError = 1;
        return 0.0;
    }

    text = end;

    return value;
}

static double Factor(void)
{
    double result;

    SkipSpaces();

    if (*text == '+')
    {
        text++;
        return Factor();
    }

    if (*text == '-')
    {
        text++;
        return -Factor();
    }

    if (*text == '(')
    {
        text++;

        result = Expression();

        SkipSpaces();

        if (*text != ')')
        {
            parserError = 1;
            return 0.0;
        }

        text++;

        return result;
    }

    if (isalpha((unsigned char)*text))
    {
        char name[20];
        int i = 0;

        while (isalpha((unsigned char)*text) && i < 19)
        {
            name[i++] = (char)tolower(
                (unsigned char)*text
            );

            text++;
        }

        name[i] = '\0';

        if (strcmp(name, "x") == 0)
        {
            return valueX;
        }

        if (strcmp(name, "pi") == 0)
        {
            return 3.14159265358979323846;
        }

        if (strcmp(name, "e") == 0)
        {
            return 2.71828182845904523536;
        }

        SkipSpaces();

        if (*text != '(')
        {
            parserError = 1;
            return 0.0;
        }

        text++;

        result = Expression();

        SkipSpaces();

        if (*text != ')')
        {
            parserError = 1;
            return 0.0;
        }

        text++;

        if (strcmp(name, "sin") == 0)
            return sin(result);

        if (strcmp(name, "cos") == 0)
            return cos(result);

        if (strcmp(name, "tan") == 0)
            return tan(result);

        if (strcmp(name, "exp") == 0)
            return exp(result);

        if (strcmp(name, "log10") == 0)
        {
            if (result <= 0)
            {
                parserError = 1;
                return 0.0;
            }
            return log10(result);
        }

        if (strcmp(name, "asin") == 0)
            return asin(result);

        if (strcmp(name, "acos") == 0)
            return acos(result);

        if (strcmp(name, "atan") == 0)
            return atan(result);

        if (strcmp(name, "sinh") == 0)
            return sinh(result);

        if (strcmp(name, "cosh") == 0)
            return cosh(result);

        if (strcmp(name, "sqrt") == 0)
        {
            if (result < 0)
            {
                parserError = 1;
                return 0.0;
            }

            return sqrt(result);
        }

        if (strcmp(name, "log") == 0)
        {
            if (result <= 0)
            {
                parserError = 1;
                return 0.0;
            }

            return log(result);
        }

        if (strcmp(name, "abs") == 0)
            return fabs(result);

        parserError = 1;

        return 0.0;
    }

    return Number();
}

static double Power(void)
{
    double left;
    double right;

    left = Factor();

    SkipSpaces();

    if (*text == '^')
    {
        text++;

        right = Power();

        left = pow(left, right);
    }

    return left;
}

static double Term(void)
{
    double result;
    double value;

    result = Power();

    while (!parserError)
    {
        SkipSpaces();

        if (*text == '*')
        {
            text++;

            value = Power();

            result *= value;
        }
        else if (*text == '/')
        {
            text++;

            value = Power();

            if (fabs(value) < 1e-15)
            {
                parserError = 1;
                return 0.0;
            }

            result /= value;
        }
        else
        {
            break;
        }
    }

    return result;
}

static double Expression(void)
{
    double result;
    double value;

    result = Term();

    while (!parserError)
    {
        SkipSpaces();

        if (*text == '+')
        {
            text++;

            value = Term();

            result += value;
        }
        else if (*text == '-')
        {
            text++;

            value = Term();

            result -= value;
        }
        else
        {
            break;
        }
    }

    return result;
}

void FunctionInit(MathFunction *function)
{
    if (!function)
        return;

    function->expression[0] = '\0';
    function->valid = 0;
}

void FunctionSet(
    MathFunction *function,
    const char *expression
)
{
    if (!function)
        return;

    if (!expression)
    {
        FunctionInit(function);
        return;
    }

    PreprocessExpression(
        expression,
        function->expression,
        FUNCTION_MAX
    );

    function->valid = 1;
}

double FunctionEvaluate(
    MathFunction *function,
    double x
)
{
    double result;

    if (!function || !function->valid)
        return 0.0;

    text = function->expression;
    valueX = x;
    parserError = 0;

    result = Expression();

    SkipSpaces();

    if (*text != '\0')
        parserError = 1;

    if (parserError)
        return NAN;

    return result;
}

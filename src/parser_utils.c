#include "parser_utils.h"
#include <ctype.h>

void PreprocesarExpresion(const char *origen, char *destino, size_t maxLen) {
    size_t j = 0;
    for (size_t i = 0; origen[i] != '\0' && j < maxLen - 1; i++) {
        char actual = origen[i];
        destino[j++] = actual;

        if (origen[i + 1] != '\0') {
            char siguiente = origen[i + 1];

            int esActualDigito = isdigit((unsigned char)actual);
            int esActualVarLetra = (actual == 'x' || actual == 'X' || actual == 'e' || actual == 'E');
            int esActualCierreParentesis = (actual == ')');

            int esSiguienteDigito = isdigit((unsigned char)siguiente);
            int esSiguienteVarLetra = (siguiente == 'x' || siguiente == 'X' || isalpha((unsigned char)siguiente));
            int esSiguienteAperturaParentesis = (siguiente == '(');

            if ((esActualDigito && (esSiguienteVarLetra || esSiguienteAperturaParentesis)) ||
                (esActualVarLetra && (esSiguienteDigito || esSiguienteVarLetra || esSiguienteAperturaParentesis)) ||
                (esActualCierreParentesis && (esSiguienteDigito || esSiguienteVarLetra || esSiguienteAperturaParentesis)))
            {
                if (j < maxLen - 1) {
                    destino[j++] = '*';
                }
            }
        }
    }
    destino[j] = '\0';
}

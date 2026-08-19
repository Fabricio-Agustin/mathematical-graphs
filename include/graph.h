#ifndef GRAPH_H
#define GRAPH_H

#include <windows.h>

#include "coordinates.h"
#include "function.h"

void DibujarGrafico(
    HDC hdc,
    int ancho,
    int alto,
    GraphView *vista,
    MathFunction *funcion
);

#endif

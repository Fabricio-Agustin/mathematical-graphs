#ifndef EXTREMES_H
#define EXTREMES_H

#include <windows.h>
#include "coordinates.h"
#include "function.h"

// Escanea el área visible para encontrar los puntos máximos y mínimos y los dibuja
void DrawExtremes(
    HDC hdc,
    int width,
    int height,
    GraphView *view,
    MathFunction *function
);

#endif

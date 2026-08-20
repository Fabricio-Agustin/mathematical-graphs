#ifndef CURSOR_INFO_H
#define CURSOR_INFO_H

#include <windows.h>
#include "coordinates.h"
#include "function.h"

void DrawCursorInfo(
    HDC hdc,
    int width,
    int height,
    GraphView *view,
    MathFunction *function,
    int mouseX,
    int mouseY
);

#endif

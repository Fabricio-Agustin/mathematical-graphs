#include <math.h>
#include <stdio.h>
#include "graph.h"

// Función auxiliar para dibujar texto sin colisionar con la API de Windows
static void DrawGraphText(HDC hdc, int x, int y, const char *text)
{
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(100, 100, 100));
    TextOutA(hdc, x, y, text, (int)strlen(text));
}

void DrawGraph(
    HDC hdc,
    int width,
    int height,
    GraphView *view,
    MathFunction *function
)
{
    // 1. Limpiar el fondo con color blanco
    HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT clientRect = {0, 0, width, height};
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // 2. Dibujar la cuadrícula, los ejes y los números
    HPEN gridPen = CreatePen(PS_DOT, 1, RGB(220, 220, 220));
    HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

    int originX = ScreenX(0.0, width, view);
    int originY = ScreenY(0.0, height, view);

    // Dibujar líneas de la cuadrícula y etiquetas numéricas basadas en la escala
    double step = 1.0;
    if (view->scale < 20.0) step = 5.0;
    if (view->scale < 10.0) step = 10.0;
    if (view->scale > 100.0) step = 0.5;
    if (view->scale > 300.0) step = 0.1;

    // Eje X (Verticales de la cuadrícula y números)
    double minX = WorldX(0, width, view);
    double maxX = WorldX(width, width, view);
    double startX = ceil(minX / step) * step;

    for (double x = startX; x <= maxX; x += step)
    {
        int px = ScreenX(x, width, view);

        // Línea de cuadrícula vertical
        SelectObject(hdc, gridPen);
        MoveToEx(hdc, px, 0, NULL);
        LineTo(hdc, px, height);

        // Dibujar número en el eje X si está visible verticalmente
        if (originY >= 0 && originY <= height && fabs(x) > 0.001)
        {
            char text[32];
            sprintf(text, "%.1f", x);
            DrawGraphText(hdc, px - 10, originY + 5, text);
        }
    }

    // Eje Y (Horizontales de la cuadrícula y números)
    double minY = WorldY(height, height, view);
    double maxY = WorldY(0, height, view);
    double startY = ceil(minY / step) * step;

    for (double y = startY; y <= maxY; y += step)
    {
        int py = ScreenY(y, height, view);

        // Línea de cuadrícula horizontal
        SelectObject(hdc, gridPen);
        MoveToEx(hdc, 0, py, NULL);
        LineTo(hdc, width, py);

        // Dibujar número en el eje Y si está visible horizontalmente
        if (originX >= 0 && originX <= width && fabs(y) > 0.001)
        {
            char text[32];
            sprintf(text, "%.1f", y);
            DrawGraphText(hdc, originX + 5, py - 8, text);
        }
    }

    // Dibujar Ejes principales (X e Y marcados en negrita)
    SelectObject(hdc, axisPen);

    if (originX >= 0 && originX <= width)
    {
        MoveToEx(hdc, originX, 0, NULL);
        LineTo(hdc, originX, height);
    }

    if (originY >= 0 && originY <= height)
    {
        MoveToEx(hdc, 0, originY, NULL);
        LineTo(hdc, width, originY);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
    DeleteObject(axisPen);

    // 3. Dibujar la función matemática si es válida
    if (function->valid)
    {
        HPEN funcPen = CreatePen(PS_SOLID, 2, RGB(0, 102, 204));
        oldPen = (HPEN)SelectObject(hdc, funcPen);

        int firstPoint = 1;
        int prevScreenY = 0;

        for (int px = 0; px < width; px++)
        {
            double xVal = WorldX(px, width, view);
            double yVal = FunctionEvaluate(function, xVal);

            if (isnan(yVal) || isinf(yVal))
            {
                firstPoint = 1;
                continue;
            }

            int py = ScreenY(yVal, height, view);

            if (!firstPoint && abs(py - prevScreenY) > height * 2)
            {
                firstPoint = 1;
                continue;
            }

            if (firstPoint)
            {
                MoveToEx(hdc, px, py, NULL);
                firstPoint = 0;
            }
            else
            {
                LineTo(hdc, px, py);
            }

            prevScreenY = py;
        }

        SelectObject(hdc, oldPen);
        DeleteObject(funcPen);
    }
}

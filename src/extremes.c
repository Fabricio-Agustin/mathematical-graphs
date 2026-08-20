#include <math.h>
#include <stdio.h>
#include "extremes.h"

void DrawExtremes(
    HDC hdc,
    int width,
    int height,
    GraphView *view,
    MathFunction *function
)
{
    if (!function || !function->valid)
        return;

    double minX = WorldX(0, width, view);
    double maxX = WorldX(width, width, view);

    double maxVal = -INFINITY;
    double maxXVal = 0.0;
    int foundMax = 0;

    double minVal = INFINITY;
    double minXVal = 0.0;
    int foundMin = 0;

    for (int px = 0; px < width; px += 2)
    {
        double xVal = WorldX(px, width, view);
        double yVal = FunctionEvaluate(function, xVal);

        if (isnan(yVal) || isinf(yVal))
            continue;

        if (yVal > maxVal)
        {
            maxVal = yVal;
            maxXVal = xVal;
            foundMax = 1;
        }

        if (yVal < minVal)
        {
            minVal = yVal;
            minXVal = xVal;
            foundMin = 1;
        }
    }

    SetBkMode(hdc, TRANSPARENT);

    if (foundMax)
    {
        int screenX = ScreenX(maxXVal, width, view);
        int screenY = ScreenY(maxVal, height, view);

        if (screenY >= 0 && screenY <= height)
        {
            HBRUSH greenBrush = CreateSolidBrush(RGB(0, 180, 0));
            HPEN greenPen = CreatePen(PS_SOLID, 1, RGB(0, 120, 0));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, greenBrush);
            HPEN oldPen = (HPEN)SelectObject(hdc, greenPen);

            Ellipse(hdc, screenX - 6, screenY - 6, screenX + 6, screenY + 6);

            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(greenBrush);
            DeleteObject(greenPen);

            char text[64];
            sprintf(text, "Max: (%.2f, %.2f)", maxXVal, maxVal);
            SetTextColor(hdc, RGB(0, 120, 0));

            RECT rect = { screenX + 10, screenY - 15, screenX + 150, screenY + 15 };
            DrawTextA(hdc, text, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    if (foundMin)
    {
        int screenX = ScreenX(minXVal, width, view);
        int screenY = ScreenY(minVal, height, view);

        if (screenY >= 0 && screenY <= height)
        {
            HBRUSH orangeBrush = CreateSolidBrush(RGB(220, 80, 0));
            HPEN orangePen = CreatePen(PS_SOLID, 1, RGB(160, 50, 0));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, orangeBrush);
            HPEN oldPen = (HPEN)SelectObject(hdc, orangePen);

            Ellipse(hdc, screenX - 6, screenY - 6, screenX + 6, screenY + 6);

            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(orangeBrush);
            DeleteObject(orangePen);

            char text[64];
            sprintf(text, "Min: (%.2f, %.2f)", minXVal, minVal);
            SetTextColor(hdc, RGB(180, 50, 0));

            RECT rect = { screenX + 10, screenY - 15, screenX + 150, screenY + 15 };
            DrawTextA(hdc, text, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

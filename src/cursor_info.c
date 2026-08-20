#include <math.h>
#include <stdio.h>
#include "cursor_info.h"

void DrawCursorInfo(
    HDC hdc,
    int width,
    int height,
    GraphView *view,
    MathFunction *function,
    int mouseX,
    int mouseY
)

{
    if (mouseX < 0 || mouseY < 0) return;

    double xVal = WorldX(mouseX, width, view);

    double yVal = FunctionEvaluate(function, xVal);

    if (isnan(yVal) || isinf(yVal)) return;

    int screenX = ScreenX(xVal, width, view);
    int screenY = ScreenY(yVal, height, view);

    if (screenY < 0 || screenY > height) return;

    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    HPEN redPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
    HPEN oldPen = (HPEN)SelectObject(hdc, redPen);

    Ellipse(hdc, screenX - 5, screenY - 5, screenX + 5, screenY + 5);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(redBrush);
    DeleteObject(redPen);

    char coordsText[64];
    sprintf(coordsText, "X: %.3f\nY: %.3f", xVal, yVal);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));

    RECT textRect = { screenX + 12, screenY - 22, screenX + 130, screenY + 22 };

    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
    oldBrush = (HBRUSH)SelectObject(hdc, whiteBrush);
    oldPen = (HPEN)SelectObject(hdc, borderPen);

    Rectangle(hdc, screenX + 10, screenY - 25, screenX + 125, screenY + 25);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(whiteBrush);
    DeleteObject(borderPen);

    DrawTextA(hdc, coordsText, -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

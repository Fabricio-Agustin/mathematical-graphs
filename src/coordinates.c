#include "coordinates.h"

int ScreenX(double x, int width, GraphView *view)
{
    return (int)((x - view->centerX) * view->scale + width / 2.0);
}

int ScreenY(double y, int height, GraphView *view)
{
    return (int)(height / 2.0 - (y - view->centerY) * view->scale);
}

double WorldX(int x, int width, GraphView *view)
{
    return (x - width / 2.0) / view->scale + view->centerX;
}

double WorldY(int y, int height, GraphView *view)
{
    return (height / 2.0 - y) / view->scale + view->centerY;
}

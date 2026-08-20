#ifndef COORDINATES_H
#define COORDINATES_H

typedef struct
{
    double scale;
    double centerX;
    double centerY;

} GraphView;

int ScreenX(
    double x,
    int width,
    GraphView *view
);

int ScreenY(
    double y,
    int height,
    GraphView *view
);

double WorldX(
    int x,
    int width,
    GraphView *view
);

double WorldY(
    int y,
    int height,
    GraphView *view
);

#endif

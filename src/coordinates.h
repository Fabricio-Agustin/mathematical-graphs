#ifndef COORDINATES_H
#define COORDINATES_H

typedef struct
{
    double escala;
    double centroX;
    double centroY;
} GraphView;

int PantallaX(double x, int ancho, GraphView *vista);
int PantallaY(double y, int alto, GraphView *vista);

double MundoX(int x, int ancho, GraphView *vista);
double MundoY(int y, int alto, GraphView *vista);

#endif

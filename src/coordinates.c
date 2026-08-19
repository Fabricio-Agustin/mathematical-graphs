#include "coordinates.h"

int PantallaX(double x, int ancho, GraphView *vista)
{
    return (int)(
        ancho / 2.0 +
        (x - vista->centroX) * vista->escala
    );
}

int PantallaY(double y, int alto, GraphView *vista)
{
    return (int)(
        alto / 2.0 -
        (y - vista->centroY) * vista->escala
    );
}

double MundoX(int x, int ancho, GraphView *vista)
{
    return vista->centroX +
        (x - ancho / 2.0) / vista->escala;
}

double MundoY(int y, int alto, GraphView *vista)
{
    return vista->centroY -
        (y - alto / 2.0) / vista->escala;
}

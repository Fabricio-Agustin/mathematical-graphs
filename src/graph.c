#include "graph.h"

#include <windows.h>
#include <math.h>
#include <stdio.h>

static void DibujarTexto(
    HDC hdc,
    int x,
    int y,
    const char *texto,
    COLORREF color
)
{
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);

    TextOutA(
        hdc,
        x,
        y,
        texto,
        lstrlenA(texto)
    );
}

static void DibujarCuadricula(
    HDC hdc,
    int ancho,
    int alto,
    GraphView *vista
)
{
    HPEN lapiz;
    HPEN anterior;

    lapiz = CreatePen(
        PS_SOLID,
        1,
        RGB(232, 232, 232)
    );

    anterior = (HPEN)SelectObject(
        hdc,
        lapiz
    );

    double izquierda = MundoX(0, ancho, vista);
    double derecha = MundoX(ancho, ancho, vista);
    double arriba = MundoY(0, alto, vista);
    double abajo = MundoY(alto, alto, vista);

    int inicioX = (int)floor(izquierda);
    int finX = (int)ceil(derecha);

    int inicioY = (int)floor(abajo);
    int finY = (int)ceil(arriba);

    int x;
    int y;

    for (x = inicioX; x <= finX; x++)
    {
        int px = PantallaX(x, ancho, vista);

        MoveToEx(hdc, px, 0, NULL);
        LineTo(hdc, px, alto);
    }

    for (y = inicioY; y <= finY; y++)
    {
        int py = PantallaY(y, alto, vista);

        MoveToEx(hdc, 0, py, NULL);
        LineTo(hdc, ancho, py);
    }

    SelectObject(hdc, anterior);
    DeleteObject(lapiz);
}

static void DibujarEjes(
    HDC hdc,
    int ancho,
    int alto,
    GraphView *vista
)
{
    HPEN lapiz;
    HPEN anterior;

    lapiz = CreatePen(
        PS_SOLID,
        2,
        RGB(30, 30, 30)
    );

    anterior = (HPEN)SelectObject(
        hdc,
        lapiz
    );

    int ejeX = PantallaY(0, alto, vista);
    int ejeY = PantallaX(0, ancho, vista);

    MoveToEx(hdc, 0, ejeX, NULL);
    LineTo(hdc, ancho, ejeX);

    MoveToEx(
        hdc,
        ancho - 12,
        ejeX - 6,
        NULL
    );

    LineTo(hdc, ancho, ejeX);

    LineTo(
        hdc,
        ancho - 12,
        ejeX + 6
    );

    MoveToEx(hdc, ejeY, 0, NULL);
    LineTo(hdc, ejeY, alto);

    MoveToEx(
        hdc,
        ejeY - 6,
        12,
        NULL
    );

    LineTo(hdc, ejeY, 0);

    LineTo(
        hdc,
        ejeY + 6,
        12
    );

    SelectObject(hdc, anterior);
    DeleteObject(lapiz);

    DibujarTexto(
        hdc,
        ancho - 25,
        ejeX + 8,
        "X",
        RGB(20, 20, 20)
    );

    DibujarTexto(
        hdc,
        ejeY + 8,
        5,
        "Y",
        RGB(20, 20, 20)
    );
}

static void DibujarNumeros(
    HDC hdc,
    int ancho,
    int alto,
    GraphView *vista
)
{
    double izquierda;
    double derecha;
    double arriba;
    double abajo;

    int inicioX;
    int finX;
    int inicioY;
    int finY;

    int ejeX;
    int ejeY;

    char texto[50];

    izquierda = MundoX(0, ancho, vista);
    derecha = MundoX(ancho, ancho, vista);

    arriba = MundoY(0, alto, vista);
    abajo = MundoY(alto, alto, vista);

    inicioX = (int)ceil(izquierda);
    finX = (int)floor(derecha);

    inicioY = (int)ceil(abajo);
    finY = (int)floor(arriba);

    ejeX = PantallaY(0, alto, vista);
    ejeY = PantallaX(0, ancho, vista);

    for (int x = inicioX; x <= finX; x++)
    {
        int px;

        if (x == 0)
            continue;

        px = PantallaX(x, ancho, vista);

        sprintf(texto, "%d", x);

        DibujarTexto(
            hdc,
            px + 4,
            ejeX + 5,
            texto,
            RGB(80, 80, 80)
        );
    }

    for (int y = inicioY; y <= finY; y++)
    {
        int py;

        if (y == 0)
            continue;

        py = PantallaY(y, alto, vista);

        sprintf(texto, "%d", y);

        DibujarTexto(
            hdc,
            ejeY + 7,
            py - 8,
            texto,
            RGB(80, 80, 80)
        );
    }

    DibujarTexto(
        hdc,
        ejeY + 5,
        ejeX + 5,
        "0",
        RGB(80, 80, 80)
    );
}

static void DibujarFuncion(
    HDC hdc,
    int ancho,
    int alto,
    GraphView *vista,
    MathFunction *funcion
)
{
    HPEN lapiz;
    HPEN anterior;

    BOOL primerPunto = TRUE;

    lapiz = CreatePen(
        PS_SOLID,
        3,
        RGB(30, 100, 220)
    );

    anterior = (HPEN)SelectObject(
        hdc,
        lapiz
    );

    for (int px = 0; px < ancho; px++)
    {
        double x;
        double y;
        int py;

        x = MundoX(px, ancho, vista);
        y = FunctionEvaluate(funcion, x);

        if (isnan(y) || isinf(y))
        {
            primerPunto = TRUE;
            continue;
        }

        py = PantallaY(y, alto, vista);

        if (py < -10000 || py > alto + 10000)
        {
            primerPunto = TRUE;
            continue;
        }

        if (primerPunto)
        {
            MoveToEx(
                hdc,
                px,
                py,
                NULL
            );

            primerPunto = FALSE;
        }
        else
        {
            LineTo(
                hdc,
                px,
                py
            );
        }
    }

    SelectObject(hdc, anterior);
    DeleteObject(lapiz);
}

void DibujarGrafico(
    HDC hdc,
    int ancho,
    int alto,
    GraphView *vista,
    MathFunction *funcion
)
{
    HBRUSH fondo;
    RECT rect;

    fondo = CreateSolidBrush(
        RGB(255, 255, 255)
    );

    rect.left = 0;
    rect.top = 0;
    rect.right = ancho;
    rect.bottom = alto;

    FillRect(
        hdc,
        &rect,
        fondo
    );

    DeleteObject(fondo);

    DibujarCuadricula(
        hdc,
        ancho,
        alto,
        vista
    );

    DibujarEjes(
        hdc,
        ancho,
        alto,
        vista
    );

    DibujarNumeros(
        hdc,
        ancho,
        alto,
        vista
    );

    if (funcion->valida)
    {
        DibujarFuncion(
            hdc,
            ancho,
            alto,
            vista,
            funcion
        );
    }
}

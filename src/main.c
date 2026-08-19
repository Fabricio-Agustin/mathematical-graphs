#include <windows.h>

#include "window.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    (void)hPrevInstance;
    (void)lpCmdLine;

    return IniciarVentana(
        hInstance,
        nCmdShow
    );
}

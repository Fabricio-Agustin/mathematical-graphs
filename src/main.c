#include <windows.h>

#include "../include/window.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    (void)hPrevInstance;
    (void)lpCmdLine;

    return InitializeWindow(
        hInstance,
        nCmdShow
    );
}

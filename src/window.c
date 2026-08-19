#include "window.h"

#include "graph.h"
#include "coordinates.h"
#include "function.h"

#include <windows.h>
#include <stdio.h>

#define ID_FUNCTION 1001
#define ID_GRAPH    1002
#define ID_CLEAR    1003

#define GRAPH_TOP 55

static HINSTANCE g_hInstance;

static HWND g_hFunction;
static HWND g_hGraphButton;
static HWND g_hClearButton;

static GraphView g_vista;
static MathFunction g_funcion;

static BOOL g_arrastrando = FALSE;

static int g_mouseAnteriorX = 0;
static int g_mouseAnteriorY = 0;

static void ActualizarFuncion(HWND hWnd)
{
    char texto[256];

    GetWindowTextA(
        g_hFunction,
        texto,
        sizeof(texto)
    );

    if (texto[0] == '\0')
    {
        g_funcion.valida = 0;

        InvalidateRect(
            hWnd,
            NULL,
            FALSE
        );

        return;
    }

    FunctionSet(
        &g_funcion,
        texto
    );

    InvalidateRect(
        hWnd,
        NULL,
        FALSE
    );
}

static void ResetVista(HWND hWnd)
{
    g_vista.escala = 40.0;
    g_vista.centroX = 0.0;
    g_vista.centroY = 0.0;

    InvalidateRect(
        hWnd,
        NULL,
        FALSE
    );
}

static LRESULT CALLBACK WindowProc(
    HWND hWnd,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (mensaje)
    {
        case WM_CREATE:
        {
            HFONT fuente;

            g_vista.escala = 40.0;
            g_vista.centroX = 0.0;
            g_vista.centroY = 0.0;

            FunctionInit(&g_funcion);

            g_hFunction = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD |
                WS_VISIBLE |
                ES_AUTOHSCROLL,
                10,
                15,
                500,
                28,
                hWnd,
                (HMENU)ID_FUNCTION,
                g_hInstance,
                NULL
            );

            g_hGraphButton = CreateWindowExA(
                0,
                "BUTTON",
                "Graficar",
                WS_CHILD |
                WS_VISIBLE |
                BS_PUSHBUTTON,
                520,
                15,
                100,
                28,
                hWnd,
                (HMENU)ID_GRAPH,
                g_hInstance,
                NULL
            );

            g_hClearButton = CreateWindowExA(
                0,
                "BUTTON",
                "Limpiar",
                WS_CHILD |
                WS_VISIBLE |
                BS_PUSHBUTTON,
                630,
                15,
                100,
                28,
                hWnd,
                (HMENU)ID_CLEAR,
                g_hInstance,
                NULL
            );

            fuente = (HFONT)GetStockObject(
                DEFAULT_GUI_FONT
            );

            SendMessage(
                g_hFunction,
                WM_SETFONT,
                (WPARAM)fuente,
                TRUE
            );

            SendMessage(
                g_hGraphButton,
                WM_SETFONT,
                (WPARAM)fuente,
                TRUE
            );

            SendMessage(
                g_hClearButton,
                WM_SETFONT,
                (WPARAM)fuente,
                TRUE
            );

            return 0;
        }

        case WM_SIZE:
        {
            int ancho = LOWORD(lParam);

            if (ancho > 200)
            {
                MoveWindow(
                    g_hFunction,
                    10,
                    15,
                    ancho - 240,
                    28,
                    TRUE
                );

                MoveWindow(
                    g_hGraphButton,
                    ancho - 220,
                    15,
                    100,
                    28,
                    TRUE
                );

                MoveWindow(
                    g_hClearButton,
                    ancho - 110,
                    15,
                    100,
                    28,
                    TRUE
                );
            }

            InvalidateRect(
                hWnd,
                NULL,
                FALSE
            );

            return 0;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case ID_GRAPH:
                    ActualizarFuncion(hWnd);
                    return 0;

                case ID_CLEAR:
                    SetWindowTextA(
                        g_hFunction,
                        ""
                    );

                    g_funcion.valida = 0;

                    InvalidateRect(
                        hWnd,
                        NULL,
                        FALSE
                    );

                    return 0;
            }

            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rect;

            HDC hdc = BeginPaint(
                hWnd,
                &ps
            );

            GetClientRect(
                hWnd,
                &rect
            );

            if (rect.bottom > GRAPH_TOP)
            {
                int ancho = rect.right;
                int alto = rect.bottom - GRAPH_TOP;

                SaveDC(hdc);

                SetViewportOrgEx(
                    hdc,
                    0,
                    GRAPH_TOP,
                    NULL
                );

                DibujarGrafico(
                    hdc,
                    ancho,
                    alto,
                    &g_vista,
                    &g_funcion
                );

                RestoreDC(
                    hdc,
                    -1
                );
            }

            EndPaint(
                hWnd,
                &ps
            );

            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            g_arrastrando = TRUE;

            g_mouseAnteriorX = LOWORD(lParam);
            g_mouseAnteriorY = HIWORD(lParam) - GRAPH_TOP;

            SetCapture(hWnd);

            return 0;
        }

        case WM_LBUTTONUP:
        {
            g_arrastrando = FALSE;

            ReleaseCapture();

            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (g_arrastrando)
            {
                int x;
                int y;

                int dx;
                int dy;

                x = LOWORD(lParam);
                y = HIWORD(lParam) - GRAPH_TOP;

                dx = x - g_mouseAnteriorX;
                dy = y - g_mouseAnteriorY;

                g_vista.centroX -=
                    dx / g_vista.escala;

                g_vista.centroY +=
                    dy / g_vista.escala;

                g_mouseAnteriorX = x;
                g_mouseAnteriorY = y;

                InvalidateRect(
                    hWnd,
                    NULL,
                    FALSE
                );
            }

            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            int delta;

            POINT mouse;
            RECT rect;

            double antesX;
            double antesY;

            double despuesX;
            double despuesY;

            delta = GET_WHEEL_DELTA_WPARAM(wParam);

            mouse.x = LOWORD(lParam);
            mouse.y = HIWORD(lParam);

            ScreenToClient(
                hWnd,
                &mouse
            );

            mouse.y -= GRAPH_TOP;

            GetClientRect(
                hWnd,
                &rect
            );

            int alto = rect.bottom - GRAPH_TOP;

            if (alto <= 0)
                return 0;

            antesX = MundoX(
                mouse.x,
                rect.right,
                &g_vista
            );

            antesY = MundoY(
                mouse.y,
                alto,
                &g_vista
            );

            if (delta > 0)
                g_vista.escala *= 1.15;
            else
                g_vista.escala /= 1.15;

            if (g_vista.escala < 5.0)
                g_vista.escala = 5.0;

            if (g_vista.escala > 500.0)
                g_vista.escala = 500.0;

            despuesX = MundoX(
                mouse.x,
                rect.right,
                &g_vista
            );

            despuesY = MundoY(
                mouse.y,
                alto,
                &g_vista
            );

            g_vista.centroX +=
                antesX - despuesX;

            g_vista.centroY +=
                antesY - despuesY;

            InvalidateRect(
                hWnd,
                NULL,
                FALSE
            );

            return 0;
        }

        case WM_KEYDOWN:
        {
            if (wParam == 'R')
            {
                ResetVista(hWnd);
            }

            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcA(
        hWnd,
        mensaje,
        wParam,
        lParam
    );
}

int IniciarVentana(
    HINSTANCE hInstance,
    int nCmdShow
)
{
    const char CLASS_NAME[] = "MathGraphWindow";

    WNDCLASSA wc;
    HWND hWnd;
    MSG msg;

    g_hInstance = hInstance;

    ZeroMemory(
        &wc,
        sizeof(wc)
    );

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorA(
        NULL,
        IDC_ARROW
    );
    wc.hIcon = LoadIconA(
        NULL,
        IDI_APPLICATION
    );
    wc.hbrBackground =
        (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(
            NULL,
            "No se pudo registrar la ventana.",
            "MathGraph",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    hWnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "MathGraph - Graficador Matematico",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1100,
        750,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBoxA(
            NULL,
            "No se pudo crear la ventana.",
            "MathGraph",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    ShowWindow(
        hWnd,
        nCmdShow
    );

    UpdateWindow(hWnd);

    while (
        GetMessageA(
            &msg,
            NULL,
            0,
            0
        ) > 0
    )
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}

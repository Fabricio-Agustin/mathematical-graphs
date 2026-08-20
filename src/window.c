#include "window.h"

#include "graph.h"
#include "coordinates.h"
#include "function.h"
#include "extremes.h" 

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

static GraphView g_view;
static MathFunction g_function;

static BOOL g_dragging = FALSE;

static int g_previousMouseX = 0;
static int g_previousMouseY = 0;

static void UpdateFunction(HWND hWnd)
{
    char text[256];

    GetWindowTextA(
        g_hFunction,
        text,
        sizeof(text)
    );

    if (text[0] == '\0')
    {
        g_function.valid = 0;

        InvalidateRect(
            hWnd,
            NULL,
            FALSE
        );

        return;
    }

    FunctionSet(
        &g_function,
        text
    );

    InvalidateRect(
        hWnd,
        NULL,
        FALSE
    );
}

static void ResetView(HWND hWnd)
{
    g_view.scale = 40.0;
    g_view.centerX = 0.0;
    g_view.centerY = 0.0;

    InvalidateRect(
        hWnd,
        NULL,
        FALSE
    );
}

static LRESULT CALLBACK WindowProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            HFONT font;

            g_view.scale = 40.0;
            g_view.centerX = 0.0;
            g_view.centerY = 0.0;

            FunctionInit(&g_function);

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
                "Graph",
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
                "Clear",
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

            font = (HFONT)GetStockObject(
                DEFAULT_GUI_FONT
            );

            SendMessage(
                g_hFunction,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );

            SendMessage(
                g_hGraphButton,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );

            SendMessage(
                g_hClearButton,
                WM_SETFONT,
                (WPARAM)font,
                TRUE
            );

            return 0;
        }

        case WM_SIZE:
        {
            int width = LOWORD(lParam);

            if (width > 200)
            {
                MoveWindow(
                    g_hFunction,
                    10,
                    15,
                    width - 240,
                    28,
                    TRUE
                );

                MoveWindow(
                    g_hGraphButton,
                    width - 220,
                    15,
                    100,
                    28,
                    TRUE
                );

                MoveWindow(
                    g_hClearButton,
                    width - 110,
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
                    UpdateFunction(hWnd);
                    return 0;

                case ID_CLEAR:

                    SetWindowTextA(
                        g_hFunction,
                        ""
                    );

                    g_function.valid = 0;

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
                int width = rect.right;
                int height = rect.bottom - GRAPH_TOP;

                SaveDC(hdc);

                SetViewportOrgEx(
                    hdc,
                    0,
                    GRAPH_TOP,
                    NULL
                );

                DrawGraph(
                    hdc,
                    width,
                    height,
                    &g_view,
                    &g_function
                );

                DrawExtremes(
                    hdc,
                    width,
                    height,
                    &g_view,
                    &g_function
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
            g_dragging = TRUE;

            g_previousMouseX = LOWORD(lParam);
            g_previousMouseY = HIWORD(lParam) - GRAPH_TOP;

            SetCapture(hWnd);

            return 0;
        }

        case WM_LBUTTONUP:
        {
            g_dragging = FALSE;

            ReleaseCapture();

            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (g_dragging)
            {
                int x;
                int y;

                int dx;
                int dy;

                x = LOWORD(lParam);
                y = HIWORD(lParam) - GRAPH_TOP;

                dx = x - g_previousMouseX;
                dy = y - g_previousMouseY;

                g_view.centerX -=
                    dx / g_view.scale;

                g_view.centerY +=
                    dy / g_view.scale;

                g_previousMouseX = x;
                g_previousMouseY = y;

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

            double beforeX;
            double beforeY;

            double afterX;
            double afterY;

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

            int height = rect.bottom - GRAPH_TOP;

            if (height <= 0)
                return 0;

            beforeX = WorldX(
                mouse.x,
                rect.right,
                &g_view
            );

            beforeY = WorldY(
                mouse.y,
                height,
                &g_view
            );

            if (delta > 0)
                g_view.scale *= 1.15;
            else
                g_view.scale /= 1.15;

            if (g_view.scale < 5.0)
                g_view.scale = 5.0;

            if (g_view.scale > 500.0)
                g_view.scale = 500.0;

            afterX = WorldX(
                mouse.x,
                rect.right,
                &g_view
            );

            afterY = WorldY(
                mouse.y,
                height,
                &g_view
            );

            g_view.centerX +=
                beforeX - afterX;

            g_view.centerY +=
                beforeY - afterY;

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
                ResetView(hWnd);
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
        message,
        wParam,
        lParam
    );
}

int InitializeWindow(
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
            "Failed to register the window.",
            "MathGraph",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    hWnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "MathGraph - Mathematical Graphing Calculator",
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
            "Failed to create the window.",
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

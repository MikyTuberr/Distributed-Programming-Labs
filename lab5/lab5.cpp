#include "framework.h"
#include "lab5.h"
#include <Windows.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"Lab5";
WCHAR szWindowClass[MAX_LOADSTRING];

HWND g_hwndMove, g_hwndSize, g_hwndPaint, g_hwndClear;
FILE* console;

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define CREATE_WINDOW(hWnd, szWindowClass, szTitle, hInstance) \
        hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, \
                CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr); \
                if (!hWnd) return FALSE;

#define BUTTON_CLASS L"BUTTON"
#define BUTTON_STYLE WS_VISIBLE | WS_CHILD | BS_CHECKBOX
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 50

#define MOVE_NAME L"WM_MOVE"
#define MOVE_XPOS 10
#define MOVE_YPOS 10

#define PAINT_NAME L"WM_PAINT"
#define PAINT_XPOS MOVE_XPOS
#define PAINT_YPOS MOVE_YPOS + BUTTON_HEIGHT + 10

#define SIZE_NAME L"WM_SIZE"
#define SIZE_XPOS PAINT_XPOS
#define SIZE_YPOS PAINT_YPOS + BUTTON_HEIGHT + 10

#define CLEAR_NAME L"CLEAR"
#define CLEAR_XPOS SIZE_XPOS
#define CLEAR_YPOS SIZE_YPOS + BUTTON_HEIGHT + 10

#define CREATE_BUTTON(hWnd, hwnd, name, x, y, hInstance) \
    hwnd = CreateWindowW(BUTTON_CLASS, name, BUTTON_STYLE, x, y, \
        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, nullptr, hInstance, nullptr); \
    if (!hwnd) return FALSE;

#define CREATE_PUSHBUTTON(hWnd, hwnd, name, x, y, hInstance) \
    hwnd = CreateWindowW(BUTTON_CLASS, name,  WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, x, y, \
        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, nullptr, hInstance, nullptr); \
    if (!hwnd) return FALSE;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnCheckBoxClick(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT OnPaint(HWND hWnd);
LRESULT OnMove(HWND hWnd);
LRESULT OnSize(HWND hWnd);
LRESULT OnDestroy();
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB5);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd;

    CREATE_WINDOW(hWnd, szWindowClass, szTitle, hInstance);

    CREATE_BUTTON(hWnd, g_hwndMove, MOVE_NAME, MOVE_XPOS, MOVE_YPOS, hInstance);
    CREATE_BUTTON(hWnd, g_hwndPaint, PAINT_NAME, PAINT_XPOS, PAINT_YPOS, hInstance);
    CREATE_BUTTON(hWnd, g_hwndSize, SIZE_NAME, SIZE_XPOS, SIZE_YPOS, hInstance);
    CREATE_PUSHBUTTON(hWnd, g_hwndClear, CLEAR_NAME, CLEAR_XPOS, CLEAR_YPOS, hInstance);

    AllocConsole();
    freopen_s(&console, "CONOUT$", "w", stdout);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        return OnCommand(hWnd, wParam, lParam);
    case WM_PAINT:
        return OnPaint(hWnd);
    case WM_MOVE:
        return OnMove(hWnd);
    case WM_SIZE:
         return OnSize(hWnd);
    case WM_DESTROY:
        return OnDestroy();
    case WM_CLOSE:
        SendMessage(hWnd, WM_DESTROY, 0, 0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    switch (wmId)
    {
    case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        break;
    case IDM_EXIT:
        SendMessage(hWnd, WM_DESTROY, 0, 0);
        break;
    default:
        OnCheckBoxClick(hWnd, wParam, lParam);
        break;
    }
    return 0;
}

void OnCheckBoxClick(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (HIWORD(wParam) == BN_CLICKED)
    {
        HWND hwndCtl = (HWND)lParam;
        if (g_hwndClear == hwndCtl)
        {
            system("cls");
            return;
        }
        UINT currentState = SendMessage(hwndCtl, BM_GETCHECK, 0, 0);
        UINT newState = (currentState == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
        SendMessage(hwndCtl, BM_SETCHECK, newState, 0);
    }
}

LRESULT OnPaint(HWND hWnd)
{   
    UINT currentState = SendMessage(g_hwndPaint, BM_GETCHECK, 0, 0);
    if (currentState == BST_CHECKED)
    {
        fprintf(console, "WM_PAINT\n");
    }
    return 0;
}

LRESULT OnSize(HWND hWnd)
{
    UINT currentState = SendMessage(g_hwndSize, BM_GETCHECK, 0, 0);
    if (currentState == BST_CHECKED)
    {
        fprintf(console, "WM_SIZE\n");
    }
    return 0;
}

LRESULT OnMove(HWND hWnd)
{
    UINT currentState = SendMessage(g_hwndMove, BM_GETCHECK, 0, 0);
    if (currentState == BST_CHECKED)
    {
        fprintf(console, "WM_MOVE\n");
    }
    return 0;
}

LRESULT OnDestroy()
{
    fprintf(console, "WM_DESTROY\n");
    Sleep(1000);
    fclose(console);
    FreeConsole();
    Sleep(1000); 
    PostQuitMessage(0);
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

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

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        1200, 300, 500, 500, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }

    g_hwndMove = CreateWindowEx(NULL, L"BUTTON", L"WM_MOVE", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 10, 100, 50, hWnd, NULL, NULL, NULL);
    if (!g_hwndMove)
    {
        return FALSE;
    }

    g_hwndPaint = CreateWindowEx(NULL, L"BUTTON", L"WM_PAINT", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 70, 100, 50, hWnd, NULL, NULL, NULL);
    if (!g_hwndPaint)
    {
        return FALSE;
    }

    g_hwndSize = CreateWindowEx(NULL, L"BUTTON", L"WM_SIZE", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 130, 100, 50, hWnd, NULL, NULL, NULL);
    if (!g_hwndSize)
    {
        return FALSE;
    }

    g_hwndClear = CreateWindowEx(NULL, L"BUTTON", L"CLEAR", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 190, 100, 50, hWnd, NULL, NULL, NULL);
    if (!g_hwndClear)
    {
        return FALSE;
    }

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

#include "framework.h"
#include "lab5-2.h"
#include <vector>
#include <string>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"Clipboard Communication";
WCHAR szWindowClass[MAX_LOADSTRING] = L"Clipboard";
HWND g_hwndCut, g_hwndCopy, g_hwndPaste, g_hwndClear, g_hwndLogger;
UINT MY_FORMAT;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT OnCopy(HWND hWnd);
LRESULT OnPaste(HWND hWnd);
LRESULT OnCut(HWND hWnd);
LRESULT OnClear(HWND hWnd);
void CopyToClipboard(HWND hWnd, const std::wstring& text);
std::wstring PasteFromClipboard(HWND hWnd);
void ClearClipboard();

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define CREATE_WINDOW(hWnd, szWindowClass, szTitle, hInstance) \
        hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, \
                CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr); \
                if (!hWnd) return FALSE;

#define TEXTFIELD_CLASS L"EDIT"
#define TEXTFIELD_STYLE WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL 
#define TEXTFIELD_WIDTH 400
#define TEXTFIELD_HEIGHT 200

#define LOGGER_NAME L""
#define LOGGER_ID 110
#define LOGGER_XPOS 150
#define LOGGER_YPOS 10

#define CREATE_TEXTFIELD(hWnd, hwnd, name, x, y, id, hInstance) \
    hwnd = CreateWindowExW(WS_EX_CLIENTEDGE, TEXTFIELD_CLASS, name, TEXTFIELD_STYLE, x, y, \
        TEXTFIELD_WIDTH, TEXTFIELD_HEIGHT, hWnd, HMENU(id), hInstance, NULL); \
    if (!hwnd) return FALSE;

#define BUTTON_CLASS L"BUTTON"
#define BUTTON_STYLE WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 50

#define CUT_NAME L"CUT"
#define CUT_ID 101
#define CUT_XPOS 10
#define CUT_YPOS 10

#define COPY_NAME L"COPY"
#define COPY_ID 102
#define COPY_XPOS CUT_XPOS
#define COPY_YPOS CUT_YPOS + BUTTON_HEIGHT + 10

#define PASTE_NAME L"PASTE"
#define PASTE_ID 103
#define PASTE_XPOS CUT_XPOS
#define PASTE_YPOS COPY_YPOS + BUTTON_HEIGHT + 10

#define CLEAR_NAME L"CLEAR"
#define CLEAR_ID 106
#define CLEAR_XPOS CUT_XPOS
#define CLEAR_YPOS PASTE_YPOS + BUTTON_HEIGHT + 10

#define CREATE_BUTTON(hWnd, hwnd, name, x, y, id, hInstance) \
    hwnd = CreateWindowW(BUTTON_CLASS, name, BUTTON_STYLE, x, y, \
        BUTTON_WIDTH, BUTTON_HEIGHT, hWnd, HMENU(id), hInstance, NULL); \
    if (!hwnd) return FALSE;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB52, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB52));

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB52));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB52);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd;
    CREATE_WINDOW(hWnd, szWindowClass, szTitle, hInstance);

    CREATE_BUTTON(hWnd, g_hwndCut, CUT_NAME, CUT_XPOS, CUT_YPOS, CUT_ID, hInstance);
    CREATE_BUTTON(hWnd, g_hwndCopy, COPY_NAME, COPY_XPOS, COPY_YPOS, COPY_ID, hInstance);
    CREATE_BUTTON(hWnd, g_hwndPaste, PASTE_NAME, PASTE_XPOS, PASTE_YPOS, PASTE_ID, hInstance);
    CREATE_BUTTON(hWnd, g_hwndClear, CLEAR_NAME, CLEAR_XPOS, CLEAR_YPOS, CLEAR_ID, hInstance);

    CREATE_TEXTFIELD(hWnd, g_hwndLogger, LOGGER_NAME, LOGGER_XPOS, LOGGER_YPOS, LOGGER_ID, hInstance);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MY_FORMAT = RegisterClipboardFormat(L"MyCustomTextFormat");

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        return OnCommand(hWnd, message, wParam, lParam);
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    case CUT_ID:
        return OnCut(hWnd);
    case COPY_ID:
        return OnCopy(hWnd);
    case PASTE_ID:
        return OnPaste(hWnd);
    case CLEAR_ID:
        return OnClear(hWnd);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT OnCopy(HWND hWnd) {
    int textLength = GetWindowTextLength(g_hwndLogger);
    wchar_t* buffer = new wchar_t[textLength + 1];
    GetWindowText(g_hwndLogger, buffer, textLength + 1);

    std::wstring text(buffer);
    CopyToClipboard(hWnd, text);
    delete[] buffer;
    return 0;
}

LRESULT OnPaste(HWND hWnd) 
{
    std::wstring text = PasteFromClipboard(hWnd);
    SendMessage(g_hwndLogger, EM_SETSEL, -1, -1);
    SendMessage(g_hwndLogger, EM_REPLACESEL, TRUE, (LPARAM)text.c_str());
    return 0;
}

LRESULT OnCut(HWND hWnd)
{
    OnCopy(hWnd);
    SetWindowText(g_hwndLogger, L"");
    return 0;
}

LRESULT OnClear(HWND hWnd) {
    SetWindowText(g_hwndLogger, L"");
    ClearClipboard();
    return 0;
}


void CopyToClipboard(HWND hWnd, const std::wstring& text) {
    if (!OpenClipboard(hWnd))
        return;

    EmptyClipboard();

    HGLOBAL hClipboardData;
    size_t size = (text.length() + 1) * sizeof(wchar_t);

    hClipboardData = GlobalAlloc(GMEM_DDESHARE, size);
    if (hClipboardData == NULL) {
        CloseClipboard();
        return;
    }

    wchar_t* buffer = static_cast<wchar_t*>(GlobalLock(hClipboardData));
    if (buffer == NULL) {
        GlobalFree(hClipboardData);
        CloseClipboard();
        return;
    }

    memcpy(buffer, text.c_str(), size);
    GlobalUnlock(hClipboardData);
    SetClipboardData(CF_UNICODETEXT, hClipboardData);
    CloseClipboard();
}

std::wstring PasteFromClipboard(HWND hWnd) {
    if (!OpenClipboard(hWnd))
        return L"";

    HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (hClipboardData == NULL) {
        CloseClipboard();
        return L"";
    }

    wchar_t* buffer = static_cast<wchar_t*>(GlobalLock(hClipboardData));
    if (buffer == NULL) {
        CloseClipboard();
        return L"";
    }

    std::wstring text(buffer);
    GlobalUnlock(hClipboardData);
    CloseClipboard();

    return text;
}

void ClearClipboard() {
    if (!OpenClipboard(NULL))
        return;

    EmptyClipboard();
    CloseClipboard();
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
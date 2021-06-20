// SimplePacker.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "SimplePacker.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND addButton(HWND hWnd, HINSTANCE hInstance, LPTSTR, int, int);
void addListBox(HWND hWnd, HINSTANCE hInstance);




HWND hWnd;

HWND hBtnAddFile;
HWND hBtnDelFile;
HWND hBtnPacking;
HWND hBtnUnPacking;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLEPACKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    addListBox(hWnd, hInstance);
    hBtnAddFile = addButton(hWnd, hInstance, (LPTSTR)L"추 가", 10, 10);
    hBtnDelFile = addButton(hWnd, hInstance, (LPTSTR)L"제 거", 10, 120);
    hBtnDelFile = addButton(hWnd, hInstance, (LPTSTR)L"패킹 풀기 (언패킹)", 10, 615);
    hBtnPacking = addButton(hWnd, hInstance, (LPTSTR)L"패킹", 10, 725);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEPACKER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

HWND hListBox;
void addListBox(HWND hWnd, HINSTANCE hInstance) {
    hListBox = CreateWindowExW(WS_EX_ACCEPTFILES, WC_LISTBOX, szTitle, WS_VSCROLL | WS_HSCROLL | WS_BORDER | LBS_COMBOBOX | LBS_HASSTRINGS | WS_CHILD, 300, 10, 1270, 830, hWnd, nullptr, hInstance, nullptr);
    ShowWindow(hListBox, true);
}

HWND addButton(HWND hWnd, HINSTANCE hInstance, LPTSTR caption, int x, int y) {
    HWND handle;

    handle = CreateWindowExW(0L, WC_BUTTONW, nullptr, BS_CENTER | WS_CHILD, x, y, 280, 100, hWnd, nullptr, hInstance, nullptr);
    Button_SetText(handle, caption);
    ShowWindow(handle, true);

    return handle;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEPACKER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SIMPLEPACKER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ,
      0, 0, 1600, 900, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                return 0;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                return 0;
            }

            // 버튼 선택
            if (lParam == (LPARAM)hBtnAddFile) {
                IFileOpenDialog* pFileOpen;
                CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&pFileOpen);
                pFileOpen->Show(NULL);
                IShellItem* pItem; 
                if (SUCCEEDED(pFileOpen->GetResult(&pItem))) {
                    LPWSTR fileName;
                    pItem->GetDisplayName(SIGDN_FILESYSPATH, &fileName);
                    //MessageBoxW(NULL, fileName, L"File Path", MB_OK);
                    SendMessageW(hListBox, LB_ADDSTRING, NULL, (LPARAM)fileName);
                    CoTaskMemFree(fileName);
                    pItem->Release();
                    pFileOpen->Release();
                    CoUninitialize();
                }

            }

            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DROPFILES:
        WCHAR fileName[255];
        DragQueryFileW((HDROP)wParam, 0, fileName, 255);
        //MessageBoxW(hWnd, fileName, fileName, MB_OK);
        DragFinish((HDROP)wParam);
        SendMessageW(hListBox, LB_ADDSTRING, NULL, (LPARAM)fileName);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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


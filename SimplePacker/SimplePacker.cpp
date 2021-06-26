// SimplePacker.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "SimplePacker.h"
#include "SimplePackerLib.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE _hInst;                                // 현재 인스턴스입니다.
WCHAR _szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR _szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND _hWnd;
HWND _hListBox;
HWND _hBtnAddFile;
HWND _hBtnDelFile;
HWND _hBtnPacking;
HWND _hBtnUnPackingAll;
HWND _hBtnUnPackingSelected;

LPWSTR _fileName = nullptr;

int _fileNum = 0;

CLinkedList<WCHAR*> _fileFullPath;

void setListBoxHorizontalLen(HWND hWnd) {

    DWORD maxLen = 0;
    for (int fileCnt = 0; fileCnt < _fileNum; ++fileCnt) {
        DWORD len = (DWORD)SendMessageW(hWnd, LB_GETTEXTLEN, fileCnt, NULL);
        if (len > maxLen) {
            maxLen = len;
        }
    }
    SendMessageW(hWnd, LB_SETHORIZONTALEXTENT, (WPARAM)maxLen * 11, NULL);
}
void addListBox(HWND hWnd, HINSTANCE hInstance) {
    _hListBox = CreateWindowExW(WS_EX_ACCEPTFILES, WC_LISTBOX, _szTitle, WS_VSCROLL | WS_HSCROLL | WS_BORDER | LBS_DISABLENOSCROLL | LBS_EXTENDEDSEL | LBS_HASSTRINGS | WS_CHILD, 180, 10, 430, 415, hWnd, nullptr, hInstance, nullptr);
    ShowWindow(_hListBox, true);
}
HWND addButton(HWND hWnd, HINSTANCE hInstance, LPTSTR caption, int x, int y) {
    HWND handle;

    handle = CreateWindowExW(0L, WC_BUTTONW, nullptr, BS_CENTER | WS_CHILD, x, y, 150, 30, hWnd, nullptr, hInstance, nullptr);
    Button_SetText(handle, caption);
    ShowWindow(handle, true);

    return handle;
}

WCHAR* getFileNameFromDirectory(WCHAR* dir) {

    WCHAR* lastSlash = dir;
    DWORD dirCnt = 0;

    for (; dir[dirCnt] != L'\0'; ++dirCnt) {
        if ((char)dir[dirCnt] == '\\'){
            lastSlash = dir + dirCnt;
        }
    }

    DWORD len = dir + dirCnt - lastSlash - 1;
    WCHAR* fileName = (WCHAR*)malloc(sizeof(WCHAR) * len + 1);
    fileName[len] = L'\0';

    int lenCnt = 0;
    for (WCHAR* iter = lastSlash + 1; *iter != L'\0'; ++iter) {
        fileName[lenCnt++] = *iter;
    }

    return fileName;
}

WCHAR* selectFolder() {

    BROWSEINFOW bi = { 0 };

    bi.lpszTitle = L"폴더를 선택해주세요";

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    WCHAR* selectPath = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);
    ZeroMemory(selectPath, sizeof(WCHAR) * MAX_PATH);

    if (pidl != NULL)    {

        SHGetPathFromIDListW(pidl, selectPath);

        CoTaskMemFree(pidl);
    }
    else {
        return nullptr;
    }

    return selectPath;
}

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
            case IDM_NEWFILE:
                
                _fileNum = 0;
                return 0;
            case IDM_SAVEFILE:
                return 0;
            case IDM_LOAD:
                if (_fileName != nullptr) {
                    CoTaskMemFree(_fileName);
                }
                IFileOpenDialog* pFileOpen;
                CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&pFileOpen);
                pFileOpen->Show(NULL);
                IShellItem* pItem;
                if (SUCCEEDED(pFileOpen->GetResult(&pItem))) {
                    pItem->GetDisplayName(SIGDN_FILESYSPATH, &_fileName);
                    CSimplePacker::instance()->readHeader(_fileName);
                    for (DWORD fileCnt = 0; fileCnt < CSimplePacker::instance()->fileNum(_fileName); ++fileCnt) {
                        SendMessageW(_hListBox, LB_ADDSTRING, NULL, (LPARAM)CSimplePacker::instance()->_headers[fileCnt].name);
                        _fileNum += 1;
                    }
                    pItem->Release();
                    pFileOpen->Release();
                    CoUninitialize();
                }
                return 0;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                return 0;
            }

            // 버튼 선택

            if (lParam == (LPARAM)_hBtnAddFile) {
                IFileOpenDialog* pFileOpen;
                CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&pFileOpen);
                pFileOpen->Show(NULL);
                IShellItem* pItem; 
                if (SUCCEEDED(pFileOpen->GetResult(&pItem))) {
                    LPWSTR fullPath;
                    pItem->GetDisplayName(SIGDN_FILESYSPATH, &fullPath);
                    int len = wcslen(fullPath);
                    WCHAR* temp = (WCHAR*)malloc(sizeof(WCHAR) * len);
                    memcpy(temp, fullPath, sizeof(WCHAR) * len);
                    _fileFullPath.push_back(temp);
                    LPWSTR fileName = getFileNameFromDirectory(fullPath);
                    SendMessageW(_hListBox, LB_ADDSTRING, NULL, (LPARAM)fileName);
                    CoTaskMemFree(fullPath);
                    pItem->Release();
                    pFileOpen->Release();
                    CoUninitialize();
                    _fileNum += 1; 
                    setListBoxHorizontalLen(_hListBox);
                }
            }
            else if (lParam == (LPARAM)_hBtnDelFile) {

                int* idxs = (int*)malloc(sizeof(int) * _fileNum);
                __int64 selNum = SendMessageW(_hListBox, LB_GETSELITEMS, _fileNum, (LPARAM)idxs);
                if (selNum == LB_ERR) {
                    // 아무것도 선택되지 않음
                    free(idxs);
                    return 0;
                }

                for (int fileCnt = 0; fileCnt < selNum; ++fileCnt) {
                    int idx = idxs[fileCnt];
                    CLinkedList<WCHAR*>::iterator iter = _fileFullPath.begin();
                    for (int idxCnt = 0; idxCnt < idx; ++idxCnt, ++iter) {}
                    _fileFullPath.erase(iter);
                    SendMessageW(_hListBox, LB_DELETESTRING, idx, NULL);
                    _fileNum -= 1;
                }
                free(idxs);
            }
            else if (lParam == (LPARAM)_hBtnPacking) {
                                
                CSimplePacker::stFile* files = (CSimplePacker::stFile*)malloc(sizeof(CSimplePacker::stFile) * _fileNum);

                int fileCnt = 0;
                for (CLinkedList<WCHAR*>::iterator filePathIter = _fileFullPath.begin(); filePathIter != _fileFullPath.end(); ++filePathIter, ++fileCnt){
                    LPWSTR fileName = new WCHAR[255];
                    __int64 length = SendMessageW(_hListBox, LB_GETTEXT, (WPARAM)fileCnt, (LPARAM)fileName);
                    FILE* file = nullptr;
                    
                    _wfopen_s(&file, *filePathIter, L"rb");
                    files[fileCnt].header.name = fileName;
                    files[fileCnt].header.nameLen = (DWORD)wcslen(fileName);
                    fseek(file, 0, SEEK_END);
                    DWORD size = ftell(file);
                    files[fileCnt].header.fileSize = size;
                    rewind(file);
                    BYTE* data = (BYTE*)malloc(sizeof(BYTE) * size);
                    fread(data, sizeof(BYTE), size, file);
                    files[fileCnt].data = data;

                }

                CSimplePacker::instance()->packing(files, _fileNum, (WCHAR*)L"packed.sp");
                MessageBoxW(hWnd, L"패킹 완료", L"Simple Packer", MB_OK);
            }
            else if (lParam == (LPARAM)_hBtnUnPackingAll) {

                WCHAR* selectPath = selectFolder();
                if (selectPath == nullptr) {
                    // 폴더 선택 취소
                    return 0;
                }

                CSimplePacker::instance()->unpackingAll(_fileName);
                
                for (DWORD fileCnt = 0; fileCnt < _fileNum; fileCnt++) {
                    
                    FILE* saveFile;
                    DWORD pathLen = wcslen(selectPath);
                    DWORD fullPathLen = pathLen + CSimplePacker::instance()->_headers[fileCnt].nameLen + 1;
                    
                    WCHAR* fullPath = (WCHAR*)malloc(sizeof(WCHAR) * (fullPathLen + 1));
                    fullPath[fullPathLen] = *(WCHAR*)L"\0";

                    memcpy(fullPath, selectPath, sizeof(WCHAR)* pathLen);
                    memcpy(fullPath + pathLen, L"\\", 2);
                    memcpy(fullPath + pathLen + 1, CSimplePacker::instance()->_headers[fileCnt].name, CSimplePacker::instance()->_headers[fileCnt].nameLen * sizeof(WCHAR));
                    
                    _wfopen_s(&saveFile, fullPath, L"wb");
                    fwrite(CSimplePacker::instance()->_files[fileCnt].data, CSimplePacker::instance()->_headers->fileSize, 1, saveFile);
                    fclose(saveFile);
                    
                }

                MessageBoxW(hWnd, L"언패킹 완료 !", L"SimplePacker", MB_OK);
            }
            else if (lParam == (LPARAM)_hBtnUnPackingSelected) {
                int* idxs = (int*)malloc(sizeof(int) * _fileNum);
                __int64 selNum = SendMessageW(_hListBox, LB_GETSELITEMS, _fileNum, (LPARAM)idxs);
                if (selNum == LB_ERR) {
                    // 아무것도 선택되지 않음
                    free(idxs);
                    return 0;
                }

                WCHAR* selectPath = selectFolder();
                if (selectPath == nullptr) {
                    // 폴더 선택 취소
                    return 0;
                }                

                for (__int64 selCnt = 0; selCnt < selNum; selCnt++) {
                    DWORD idx = idxs[selCnt];
                    CSimplePacker::instance()->unpackingSingleFile(_fileName, idx); FILE* saveFile;
                    DWORD pathLen = wcslen(selectPath);
                    DWORD fullPathLen = pathLen + CSimplePacker::instance()->_headers[idx].nameLen + 1;

                    WCHAR* fullPath = (WCHAR*)malloc(sizeof(WCHAR) * (fullPathLen + 1));
                    fullPath[fullPathLen] = *(WCHAR*)L"\0";

                    memcpy(fullPath, selectPath, sizeof(WCHAR)* pathLen);
                    memcpy(fullPath + pathLen, L"\\", 2);
                    memcpy(fullPath + pathLen + 1, CSimplePacker::instance()->_headers[idx].name, CSimplePacker::instance()->_headers[idx].nameLen * sizeof(WCHAR));

                    _wfopen_s(&saveFile, fullPath, L"wb");
                    fwrite(CSimplePacker::instance()->_files[idx].data, CSimplePacker::instance()->_headers->fileSize, 1, saveFile);
                    fclose(saveFile);
                }
                MessageBoxW(hWnd, L"언패킹 완료 !", L"SimplePacker", MB_OK);

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
        {
            WCHAR* fullPath = (WCHAR*)malloc(sizeof(WCHAR) * 255);
            DragQueryFileW((HDROP)wParam, 0, fullPath, 255);
            WCHAR* fileName = getFileNameFromDirectory(fullPath);
            _fileFullPath.push_back(fullPath);
            DragFinish((HDROP)wParam);
            SendMessageW(_hListBox, LB_ADDSTRING, NULL, (LPARAM)fileName);
            _fileNum += 1;
            setListBoxHorizontalLen(_hListBox);
        }
        break;
    case WM_DESTROY:
        CoTaskMemFree(_fileName);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEPACKER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEPACKER);
    wcex.lpszClassName = _szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    const DWORD width = 640;
    const DWORD height = 480;

    _hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    _hWnd = CreateWindowW(_szWindowClass, _szTitle, WS_OVERLAPPEDWINDOW,
        0, 0, width, height, nullptr, nullptr, hInstance, nullptr);

    RECT rt;
    rt.right = width;
    rt.bottom = height;

    AdjustWindowRectEx(&rt, GetWindowStyle(_hWnd), GetMenu(_hWnd) != NULL, GetWindowExStyle(_hWnd));

    MoveWindow(_hWnd, 0, 0, rt.right, rt.bottom, false);

    if (!_hWnd)
    {
        return FALSE;
    }

    ShowWindow(_hWnd, nCmdShow);
    UpdateWindow(_hWnd);

    return TRUE;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, _szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLEPACKER, _szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    addListBox(_hWnd, hInstance);
    _hBtnAddFile = addButton(_hWnd, hInstance, (LPTSTR)L"추 가", 10, 10);
    _hBtnDelFile = addButton(_hWnd, hInstance, (LPTSTR)L"제 거", 10, 50);
    _hBtnPacking = addButton(_hWnd, hInstance, (LPTSTR)L"패킹", 10, 300);
    _hBtnUnPackingAll = addButton(_hWnd, hInstance, (LPTSTR)L"전체 언패킹", 10, 340);
    _hBtnUnPackingSelected = addButton(_hWnd, hInstance, (LPTSTR)L"선택 항목 언패킹", 10, 380);

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

    return (int)msg.wParam;
}
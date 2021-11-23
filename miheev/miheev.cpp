// miheev.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "miheev.h"
#include <windowsx.h>
#include <Psapi.h>
#include <strsafe.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
DWORD dwProcessId;
HINSTANCE hInst;                                // текущий экземпляр
HWND hListBox1,hListBox2,hListBoxJobs,hEditJobName,hListBoxProcessInJob;
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
void                LoadProcessesInJobToListBox(HWND hList, ULONG_PTR* ProcessesId, DWORD cbNeeded);
BOOL                EnumProcessesInJob(HANDLE hJob, ULONG_PTR* lpidProcess, DWORD cb, LPDWORD lpcbNeeded);
BOOL                WaitProcessByID(DWORD dwProcessId, DWORD dwMilliseconds, LPDWORD LpExitCode);
void                LoadModulesToListBox(HWND hwndCtrl, DWORD dwProcessId);
void                LoadProcessesToListBox(HWND hwndCtrl);
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MIHEEV, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MIHEEV));

    MSG msg;

    // Цикл основного сообщения:
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



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MIHEEV));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MIHEEV);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   else {
           CreateWindowEx(0, L"Button", L"Ожидать завершения", WS_CHILD | WS_VISIBLE, 630, 10, 150, 40, hWnd, (HMENU)IDC_BUTTON1, hInst, NULL);
           CreateWindowEx(0, L"Button", L"Завершить процесс", WS_CHILD | WS_VISIBLE, 630, 60, 150, 40, hWnd, (HMENU)IDC_BUTTON2, hInst, NULL);
           CreateWindowEx(0, L"Button", L"Создать процесс", WS_CHILD | WS_VISIBLE, 630, 110, 150, 40, hWnd, (HMENU)IDC_BUTTON3, hInst, NULL);
           CreateWindowEx(0, L"Button", L"Добавить процесс в задание", WS_CHILD | WS_VISIBLE, 630, 160, 220, 40, hWnd, (HMENU)IDC_BUTTON5, hInst, NULL);
           hListBoxJobs = CreateWindowEx(0, L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD, 900, 10, 500, 100, hWnd, (HMENU)IDC_LIST3, hInst, NULL);
           CreateWindowEx(0, L"Button", L"Создать задание", WS_CHILD | WS_VISIBLE, 1050, 130, 200, 40, hWnd, (HMENU)IDC_BUTTON4, hInst, NULL);
           hEditJobName=CreateWindowEx(0, L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 1050, 200, 200, 40, hWnd, (HMENU)IDC_EDIT1, hInst, NULL);
           CreateWindowEx(0, L"Static", L"Введите название задания", WS_CHILD | WS_VISIBLE, 1050, 180, 200, 20, hWnd, NULL, hInst, NULL);
           CreateWindowEx(0, L"Static", L"Процессы в выбранном задании", WS_CHILD | WS_VISIBLE, 1040, 280, 225, 20, hWnd, NULL, hInst, NULL);
           hListBoxProcessInJob= CreateWindowEx(0, L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD, 900, 305, 500, 200, hWnd, (HMENU)IDC_LIST4, hInst, NULL);
           hListBox1 = CreateWindowEx(0, L"listbox", NULL, WS_CHILD| WS_VISIBLE | LBS_STANDARD, 10, 10, 600, 200, hWnd, (HMENU)IDC_LIST1, hInst, NULL);
           hListBox2 = CreateWindowEx(0, L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD, 10, 210, 600, 200, hWnd, (HMENU)IDC_LIST2, hInst, NULL);
           LoadProcessesToListBox(hListBox1);
           SetTimer(hWnd, IDC_TIMER1, 2000, 0);
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDC_LIST1: {
                int item = SendMessage(hListBox1, LB_GETCURSEL, 0, 0L);
                DWORD ProcessId = ListBox_GetItemData(hListBox1, item);
                if (ProcessId != 0) {
                    LoadModulesToListBox(hListBox2, ProcessId);
                }
                else {
                    ListBox_ResetContent(hListBox2);
                }
            }
                          break;
            case IDC_LIST3: {
                if (wmEvent== LBN_DBLCLK) {
                    int item = SendMessage(hListBoxJobs, LB_GETCURSEL, 0, 0L);
                    WCHAR JobName[30] = L"";
                    ListBox_GetText(hListBoxJobs, item, JobName);
                    HANDLE hJob = OpenJobObject(JOB_OBJECT_QUERY, FALSE, JobName);
                    ULONG_PTR lpidProcess[30];
                    DWORD lpcbNeeded = 0;
                    BOOL bRet = EnumProcessesInJob(hJob, lpidProcess, sizeof(lpidProcess), &lpcbNeeded);
                    LoadProcessesInJobToListBox(hListBoxProcessInJob, lpidProcess, lpcbNeeded);
                }
            }
                          break;
            case IDC_BUTTON1: {
                int item = SendMessage(hListBox1, LB_GETCURSEL, 0, 0L);
                if (item == LB_ERR) {
                    MessageBox(hWnd, L"Выберите процесс, который будет ожидаться", NULL,MB_OK);
                }
                else {
                    DWORD ProcessId = ListBox_GetItemData(hListBox1, item);
                    DWORD ExitCode;
                    DWORD Milliseconds = 10000;
                    BOOL WaitProcess = WaitProcessByID(ProcessId, Milliseconds, &ExitCode);
                    if (WaitProcess == FALSE) {
                        MessageBox(hWnd, L"Программе не удалось открыть процесс", NULL, MB_OK);
                    }
                    else {
                        if (ExitCode == STILL_ACTIVE) {
                            MessageBox(hWnd, L"Превышено время ожидания завершения процесса", NULL, MB_OK);
                        }
                        else { MessageBox(hWnd, L"Процесс успешно завершил работу", L"CONGRATULATIONS!!!", MB_OK);
                        }
                    }
                }
                break;
            }
            case IDC_BUTTON2: {
                int item = SendMessage(hListBox1, LB_GETCURSEL, 0, 0L);
                if (item == LB_ERR) {
                    MessageBox(hWnd, L"Выберите процесс для завершения", NULL, MB_OK);
                }
                else {
                    DWORD ProcessId = ListBox_GetItemData(hListBox1, item);
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessId);
                    if (hProcess == NULL) {
                        MessageBox(hWnd, L"Программе не удалось открыть процесс", NULL, MB_OK);
                    }
                    else {
                        TerminateProcess(hProcess, 0);
                        MessageBox(hWnd, L"Процесс завершил работу успешно", NULL, MB_OK);
                        LoadProcessesToListBox(hListBox1);
                    }
                }
            } 
                            break;
            case IDC_BUTTON3: {
                TCHAR szCmdLine[] = TEXT("cmd.exe");
                STARTUPINFO si = { sizeof(STARTUPINFO) };
                PROCESS_INFORMATION pi;
                CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
                LoadProcessesToListBox(hListBox1);
            }
                            break;
            case IDC_BUTTON4: {
                TCHAR* JobName=0;
                size_t BufSize = Edit_GetTextLength(hEditJobName) + 1;
                if (BufSize==0) MessageBox(hWnd, L"Введите название задания", NULL, MB_OK);
                else {
                    JobName = new TCHAR[BufSize];
                    Edit_GetText(hEditJobName, JobName, BufSize);
                    HANDLE JOB = CreateJobObject(NULL, JobName);
                    int iItem = ListBox_AddString(hListBoxJobs, JobName);
                    ListBox_SetItemData(hListBoxJobs, iItem, JobName);
                    Edit_SetText(hEditJobName, L"");
                }
            }
                            break;
            case IDC_BUTTON5: {
                int item = SendMessage(hListBox1, LB_GETCURSEL, 0, 0L);
                if (item==LB_ERR) MessageBox(hWnd, L"Выберите процесс", NULL, MB_OK);
                else {
                    DWORD ProcessId = ListBox_GetItemData(hListBox1, item);
                    HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, ProcessId);
                    int itemjob = SendMessage(hListBoxJobs, LB_GETCURSEL, 0, 0L);
                    if (itemjob==LB_ERR) MessageBox(hWnd, L"Выберите задание", NULL, MB_OK); 
                    else {
                        WCHAR JobName[30] = L"";
                        ListBox_GetText(hListBoxJobs, itemjob, JobName);
                        HANDLE hJob = OpenJobObject(JOB_OBJECT_ASSIGN_PROCESS, FALSE, JobName);
                        BOOL bRet = AssignProcessToJobObject(hJob, hProcess);
                        if (bRet != FALSE) MessageBox(hWnd, L"Процесс успешно добавлен", NULL, MB_OK);
                        else MessageBox(hWnd, L"Процесс не добавлен, возможно он уже состоит в задании", NULL, MB_OK);
                    }
                }
            } 
                            break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_TIMER: {
        int item = SendMessage(hListBox1, LB_GETCURSEL, 0, 0L);
        LoadProcessesToListBox(hListBox1);
        SendMessage(hListBox1, LB_SETCURSEL, item, 0);
    }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
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

// Обработчик сообщений для окна "О программе".
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

void LoadProcessesInJobToListBox(HWND hList, ULONG_PTR* ProcessesId, DWORD cbNeeded) {
    ListBox_ResetContent(hList);
    TCHAR szName[MAX_PATH], szBuffer[300];
    for (DWORD i = 0, n = cbNeeded / sizeof(ULONG_PTR); i < n; ++i)
    {
        DWORD dwProcessId = ProcessesId[i], cch = 0;
        if (0 == dwProcessId) continue;

        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

        if (NULL != hProcess)

        {
            cch = GetModuleBaseName(hProcess, NULL, szName, MAX_PATH);
            CloseHandle(hProcess);
        }

        if (0 == cch)
            StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));
        StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), szName, dwProcessId);

        int iItem = ListBox_AddString(hList, szBuffer);

        ListBox_SetItemData(hList, iItem, dwProcessId);
    }
}
void LoadProcessesToListBox(HWND hwndCtrl)
{
    ListBox_ResetContent(hwndCtrl);

    DWORD aProcessIds[1024], //Список дескрипторов процессов
        cbNeeded = 0; //Кол-во этих самых дескрипторов
    BOOL bRet = EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbNeeded);//Получаем список и размер этого списка

    if (FALSE != bRet)
    {
        TCHAR szName[MAX_PATH], szBuffer[300];
        for (DWORD i = 0, n = cbNeeded / sizeof(DWORD); i < n; ++i)
        {
            DWORD dwProcessId = aProcessIds[i], cch = 0;
            if (0 == dwProcessId) continue;

            HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

            if (NULL != hProcess)

            {
                cch = GetModuleBaseName(hProcess, NULL, szName, MAX_PATH);
                CloseHandle(hProcess);
            }

            if (0 == cch)
                StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));
            StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), szName, dwProcessId);

            int iItem = ListBox_AddString(hwndCtrl, szBuffer);

            ListBox_SetItemData(hwndCtrl, iItem, dwProcessId);
        }
    }
}
BOOL WaitProcessByID(DWORD dwProcessId, DWORD dwMilliseconds, LPDWORD LpExitCode) {
    HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
    if (hProcess == NULL) {
        return FALSE;
    }
    WaitForSingleObject(hProcess, dwMilliseconds);
    if (LpExitCode != NULL) {
        GetExitCodeProcess(hProcess, LpExitCode);
    }
    CloseHandle(hProcess);
    return TRUE;

}
BOOL EnumProcessesInJob(HANDLE hJob, ULONG_PTR* lpidProcess, DWORD cb, LPDWORD lpcbNeeded) {
    DWORD nCount = cb / sizeof(ULONG_PTR);

    if (NULL != lpidProcess && nCount > 0) {

        DWORD cbJobPIL = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST)
            + (nCount - 1) * sizeof(ULONG_PTR);

        JOBOBJECT_BASIC_PROCESS_ID_LIST* pJobPIL =
            (JOBOBJECT_BASIC_PROCESS_ID_LIST*)malloc(cbJobPIL);

        if (NULL != pJobPIL) {
            pJobPIL->NumberOfAssignedProcesses = nCount;

            BOOL bRet = QueryInformationJobObject(hJob,
                JobObjectBasicProcessIdList, pJobPIL, cbJobPIL, NULL);
           DWORD LateErr= GetLastError();
            if (FALSE != bRet) {
                nCount = pJobPIL->NumberOfProcessIdsInList;

                CopyMemory(lpidProcess, pJobPIL->ProcessIdList,
                    nCount * sizeof(ULONG_PTR));

                if (NULL != lpcbNeeded)
                    *lpcbNeeded = nCount * sizeof(ULONG_PTR);
            }
            free(pJobPIL);
            return bRet;
        }
    }
    return FALSE;
}
void LoadModulesToListBox(HWND hwndCtrl, DWORD dwProcessId)
{
    ListBox_ResetContent(hwndCtrl);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

    if (NULL != hProcess)
    {
        DWORD cb = 0;
        EnumProcessModulesEx(hProcess, NULL, 0, &cb, LIST_MODULES_ALL);

        DWORD nCount = cb / sizeof(HMODULE);

        HMODULE* hModule = new HMODULE[nCount];

        cb = nCount * sizeof(HMODULE);
        BOOL bRet = EnumProcessModulesEx(hProcess, hModule, cb, &cb, LIST_MODULES_ALL);

        if (FALSE != bRet)
        {
            TCHAR szFileName[MAX_PATH];

            for (DWORD i = 0; i < nCount; ++i)
            {
                bRet = GetModuleFileNameEx(hProcess, hModule[i], szFileName, MAX_PATH);
                if (FALSE != bRet)
                    ListBox_AddString(hwndCtrl, szFileName);
            }
        }
        delete[]hModule;

        CloseHandle(hProcess);
    }
}
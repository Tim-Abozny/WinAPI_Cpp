#define _CRT_SECURE_NO_WARNINGS

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include <string>
#include <stdio.h>
#include <TlHelp32.h>

HWND hMainDlg = NULL;
HANDLE kill = NULL;
HANDLE kill2 = NULL;

int CALLBACK MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //объ€вление переменных
    STARTUPINFO si{ 0 };
    PROCESS_INFORMATION pi = { 0 };
    HANDLE hSnap;
    HANDLE process = { 0 };
    PROCESSENTRY32 pe;
    wchar_t buf[100] = L"";
    auto appName = L"testApp.exe";
    auto appName2 = L"syncApp.exe"; // C:\\Users\\progr\\source\\repos\\testApp\\Debug\\syncApp.exe

	switch (uMsg)
	{
	case WM_INITDIALOG:
		hMainDlg = hWnd;
		PostMessage(hMainDlg, WM_APP + 1, 0, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RUN:
			CreateProcess(
				L"C:\\Users\\progr\\source\\repos\\testApp\\Debug\\testApp.exe",
				NULL, NULL, NULL, FALSE, NULL, NULL, NULL,
				&si, &pi
			);
			CreateProcess(
				L"C:\\Users\\progr\\source\\repos\\syncApp\\Debug\\syncApp.exe",
				NULL, NULL, NULL, FALSE, NULL, NULL, NULL,
				&si, &pi
			);
			return 0;

		case IDCANCEL:
			EndDialog(hWnd, 0);
			return 0;

		case IDC_RELOAD:
			SendDlgItemMessage(hWnd, IDC_PROCESSLIST, LB_RESETCONTENT, 0, 0);
			hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32);

			Process32First(hSnap, &pe);
			do
			{
				swprintf(buf, sizeof(buf), (TEXT("% s        ID\t% ld")), pe.szExeFile, pe.th32ProcessID);
				SendDlgItemMessage(hWnd, IDC_PROCESSLIST, LB_ADDSTRING, 0, (LPARAM)buf);
				process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pe.th32ProcessID);

				if (!wcscmp(pe.szExeFile, appName))
				{
					kill = process;
				}
				if (!wcscmp(pe.szExeFile, appName2))
				{
					kill2 = process;
				}
				
			} while (Process32Next(hSnap, &pe));
			CloseHandle(hSnap);

			//подчищаем мусорок
			hSnap = 0;
			pe = { 0 };
			process = 0;
			si = { 0 };
			pi = { 0 };
			return 0;

		case IDC_KILL:
			TerminateProcess(kill, 0);
			TerminateProcess(kill2, 0);

			CloseHandle(process);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return 0;
		}
		break;
	case WM_APP + 1:
		return 0;
	}

	return EXIT_SUCCESS;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDlgProc);
}

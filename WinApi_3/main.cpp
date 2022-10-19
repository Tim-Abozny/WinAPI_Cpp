#define _CRT_SECURE_NO_WARNINGS

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <tchar.h>
#include "resource.h"

const long long int nArraySize = 65536 * 2555;
int nSortState[3] = { 0 };
HWND hMainDlg = NULL;

void FillArray(int* arr, int size)
{
	for (int i = 0; i < size; ++i)
		arr[i] = rand() % 1000;
}

void SelectSort(int* arr, int size)
{
	int tmp;
	for (int i = 0; i < size; ++i)
	{
		int pos = i;
		tmp = arr[i];
		for (int j = i + 1; j < size; ++j)
		{
			if (arr[j] < tmp)
			{
				pos = j;
				tmp = arr[j];
			}
		}
		arr[pos] = arr[i];
		arr[i] = tmp;
	}
}

void BubbleSort(int* arr, int size)
{
	int tmp;

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size - 1; ++j)
		{
			if (arr[j + 1] < arr[j])
			{
				tmp = arr[j + 1];
				arr[j + 1] = arr[j];
				arr[j] = tmp;
			}
		}
	}
}

void QuickSort(int* a, int N)
{
	int i = 0, j = N;
	int temp, p;

	p = a[N >> 1];

	do
	{
		while (a[i] < p) i++;
		while (a[j] > p) j--;

		if (i <= j)
		{
			temp = a[i];
			a[i] = a[j];
			a[j] = temp;
			i++; j--;
		}
	} while (i <= j);

	if (j > 0) QuickSort(a, j);
	if (N > i) QuickSort(a + i, N - i);
}

LPCTSTR StateToString(int nState)
{
	switch (nState)
	{
		case 0: return TEXT("READY");
		case 1: return TEXT("RUNNING");
	}
	return TEXT("");
}

void UpdateDialog(int index, int state)
{
	nSortState[index] = state;
	PostMessage(hMainDlg, WM_APP + 1, 0, 0);
}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	int index = (int)lpParameter;
	try
	{
		UpdateDialog(index, 1);
		int* array = new int[nArraySize];
		FillArray(array, nArraySize);

		switch (index)
		{
		case 0:
			SelectSort(array, nArraySize);

			break;
		case 1:
			BubbleSort(array, nArraySize);

			break;
		case 2:
			QuickSort(array, nArraySize - 1);

			break;
		}

		delete[] array;

		UpdateDialog(index, 0);
		return 0;
	}
	catch (...)
	{
		UpdateDialog(index, 0);
		return 0;
	}
	
}

int CALLBACK MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

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

					SetThreadPriority(CreateThread(NULL, 0, ThreadProc, (LPVOID)0, 0, NULL), 6); // last NULL -- THid
					SetThreadPriority(CreateThread(NULL, 0, ThreadProc, (LPVOID)1, 0, NULL), 15); // last NULL -- THid
					SetThreadPriority(CreateThread(NULL, 0, ThreadProc, (LPVOID)2, 0, NULL), 1); // last NULL -- THid
					//MessageBox(hWnd, L"«¿ƒ¿… Ã≈Õ‹ÿ≈", L"¿ ‚ÓÚ ÌÂÚ :)", MB_OK | MB_ICONINFORMATION);
					return 0;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					return 0;
			}
			break;
		case WM_APP + 1:
			SetDlgItemText(hWnd, IDC_SELECTSORT, StateToString(nSortState[0]));
			SetDlgItemText(hWnd, IDC_BUBBLESORT, StateToString(nSortState[1]));
			SetDlgItemText(hWnd, IDC_QUICKSORT, StateToString(nSortState[2]));
			EnableWindow(GetDlgItem(hWnd, IDC_RUN), !(nSortState[0] || nSortState[1] || nSortState[2]));
			return 0;
	}

	return EXIT_SUCCESS;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDlgProc);
}
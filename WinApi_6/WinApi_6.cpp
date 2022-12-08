/*
	Обедающие философы.
	ОСТОРОЖНО! НЕПОЛНАЯ КАРТИНА МИРА.
	Модель упрощена:
		1. Философ требует ресурсы на достаточно строго определённое время.
			В оригинальной задаче философ может уснуть с вилками в руках, и метод обслуживания
			философов должен это учитывать. В данном случае модель просто знает что философ не
			отбитый, но и не знает, на сколько конкретно он берёт вилки.
*/

#include <Windows.h>
#include <tchar.h>
#include <Lmcons.h>
//это понятно что такое
LPCTSTR text_eats = TEXT("Eats");
LPCTSTR text_thinks = TEXT("Thinks");


struct __Philosopher_timings {
	const int msEats;		//в милисекундах
	const int msThinks;		//в милисекундах
};

const __Philosopher_timings timings[] =
{
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
	{1000, 2500},
};

const int PHILOSOPHER_NUBMER = sizeof(timings)
/ sizeof(__Philosopher_timings);

HWND hwndTextField[PHILOSOPHER_NUBMER];
HANDLE mtxForks[PHILOSOPHER_NUBMER + 1];
HANDLE mtxIAmTheOneWhoGetsServed;

/*
	МОНИТОР - это интерфейс системы-супервизора, который она предоставляет своим пользователям.
	Здесь супервизор - система, которая следит за корректностью работы с вилками.
*/

void requestForks(int num) {
	PHANDLE mutexes_addr = mtxForks;
	mutexes_addr += num;
	WaitForMultipleObjects(2, mutexes_addr, TRUE, INFINITE);
}


void releaseForks(int num) {
	ReleaseMutex(mtxForks[num]);
	ReleaseMutex(mtxForks[num]);
}


//поток-философ. кушает и думает
DWORD WINAPI philosopherThread(LPVOID lpParam) {
	//определяет свой номер
	int num = (int)lpParam;

	while (true) {
		//
		Sleep(timings[num].msThinks);
		requestForks(num);
		SetWindowText(hwndTextField[num], text_eats);
		Sleep(timings[num].msEats);
		SetWindowText(hwndTextField[num], text_thinks);
		releaseForks(num);
	}
}

void initializeWindow(HWND hMainWindow) {
	//создание текстовых полей и сохранение их хэндлов
	for (int i = 0; i < PHILOSOPHER_NUBMER; i++) {
		hwndTextField[i] = CreateWindowEx(
			0,
			TEXT("STATIC"),
			text_thinks,
			WS_VISIBLE | WS_CHILD,
			20, 10 + 20 * i, 45, 20,
			hMainWindow,
			NULL,
			(HINSTANCE)GetWindowLongPtr(hMainWindow, GWLP_HINSTANCE),
			NULL
		);
	}
}


// оконная процедура
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_CREATE:
		//инициализатор окна (создаёт текстовые поля)
		initializeWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//создание вилок
	for (int i = 0; i < PHILOSOPHER_NUBMER; i++) {
		mtxForks[i] = CreateMutex(NULL, FALSE, NULL);
	}
	//последняя вилка будет равна первой
	mtxForks[PHILOSOPHER_NUBMER] = mtxForks[0];

	//хэндлы потоков философов
	HANDLE hPhilosopherThread[PHILOSOPHER_NUBMER];
	//запуск философов
	for (int i = 0; i < PHILOSOPHER_NUBMER; i++) {
		hPhilosopherThread[i] = CreateThread(NULL, 0, philosopherThread, LPVOID(i), 0, NULL);
	}

	WNDCLASSEX wcex;
	HWND hMainWindow;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindowClass";
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);

	hMainWindow = CreateWindowEx(
		0,
		L"MainWindowClass",
		L"ВЛАДЫМЦЕВ ВАДИМ ДЕНИСОВИЧ",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 200, 300,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	PlaySound(TEXT("C:\\Users\\progr\\source\\repos\\C++\\WinAPI_Cpp\\WinApi_6\\Rick.wav"), NULL, SND_FILENAME | SND_ASYNC);

	while (GetMessage(&msg, 0, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return 0;
}
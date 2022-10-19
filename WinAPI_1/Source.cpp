#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <algorithm>
#include <string>

static std::pair<bool, HWND> AddWindow(const std::wstring&& winClass, const std::wstring&& title, HWND hParentWnd, const WNDPROC callback)
{
	UnregisterClass(winClass.c_str(), GetModuleHandle(nullptr));
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	HWND hWindow{};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // подгружаем курсор
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // иконку окна
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); // и маленьку иконку окна
	wc.lpfnWndProc = callback;
	wc.lpszClassName = winClass.c_str();
	wc.style = CS_VREDRAW | CS_HREDRAW;

	const auto create_window = [&hWindow, &winClass, &title, &hParentWnd]() -> std::pair<bool, HWND> {
		if (hWindow = CreateWindow(winClass.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, hParentWnd, nullptr, nullptr, nullptr); !hWindow)
			return { false, nullptr };

		ShowWindow(hWindow, SW_SHOWDEFAULT);
		UpdateWindow(hWindow);
		return { true, hWindow };
	};

	if (!RegisterClassEx(&wc))
		return create_window();

	return create_window();
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	static const UINT MY_DATA_MSG{ WM_USER + 1000 };
	static MSG msg{};
	static HWND hwnd{}, hButton{}, hButton1{}, hChildWnd{}, hChildEdit{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0; // эти два поля отвечают за дополнительное выделение
	wc.cbWndExtra = 0; //  памяти в классе окна. Я не использую это, но бывает нужно передать какую-то инфу в класс окна, чтобы потом эту инфу юзать.
	//соответственно, чтобы это сделать, нужно было бы запихнуть в эти переменные размер это инфы (структуры, объекта etc)
	// У каждого окна есть свой класс. По сути, всё, что идёт с "wc." -- является частью класса окна.
	// В WinApi любой объект: кнопка, поле для ввода, scrollbar -- это всё окна.
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)); // поле, принимающее дискриптор кисти, чтобы потом окрасить окошко
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // подгружаем курсор
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // иконку окна
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); // и маленьку иконку окна
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT // процедура, отвечающая за обработку сообщений
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			RECT rc{};
			GetClientRect(hWnd, &rc);

			hButton = CreateWindow(
				L"BUTTON",
				L"Навести суеты",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, 0, rc.right, 30,
				hWnd, reinterpret_cast<HMENU>(1337), nullptr, nullptr
			);

			hButton1 = CreateWindow(
				L"BUTTON",
				L"Вкинуть мысль",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, 40, rc.right, 30,
				hWnd, reinterpret_cast<HMENU>(1338), nullptr, nullptr
			);

			HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Tahoma");
			SendMessage(hButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), true);
			SendMessage(hButton1, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), true);

		}
		return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case 1337:
			{
				if (hChildWnd) DestroyWindow(hChildWnd);
				const auto [flag, hChild] = AddWindow(L"MyAppChildClass", L"Hello!", hWnd, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT {
					switch (uMsg)
					{
					case WM_CREATE:
					{
						HWND hEdit = CreateWindow(
							L"EDIT",
							L"",
							WS_CHILD | WS_BORDER | WS_VISIBLE,
							0, 0, 90, 20, hWnd, nullptr, nullptr, nullptr
						);
						hChildEdit = hEdit;
					}
					return 0;

					case MY_DATA_MSG:
					{
						MessageBox(hWnd, L"~MY_DATA!~", L"OK", MB_OK | MB_ICONINFORMATION);
					}
					return 0;
					}
					return DefWindowProc(hWnd, uMsg, wParam, lParam);
					});
				hChildWnd = hChild;
			}
			break;

			case 1338:
			{
				static std::wstring text{};
				text.resize(MAX_PATH);

				SendMessage(hChildEdit, MY_DATA_MSG, 0, 0);

				SetWindowText(hChildEdit, L"Hello!");
				GetWindowText(hChildEdit, text.data(), MAX_PATH);
				text.erase(std::remove(std::begin(text), std::end(text), 0), std::end(text));
				MessageBox(hWnd, text.c_str(), L"OK", MB_ICONINFORMATION | MB_OK);
			}
			break;
			}
		}
		return 0;

		case WM_SIZE:
		{

		}
		return 0;

		case WM_DESTROY:
		{
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	wc.lpszClassName = L"MyAppClass";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW; // стандартные стили для окна

	// регистрирую в системе класс с окном
	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	if (hwnd = CreateWindow(wc.lpszClassName, L"Основное окно", WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - 1200) / 2,
		(GetSystemMetrics(SM_CXSCREEN) - 1200) / 2,
		600, 600, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg); // необязательно для новых версий винды, но для совместимости лучше оставить
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}
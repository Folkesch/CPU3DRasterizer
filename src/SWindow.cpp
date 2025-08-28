#include "SWindow.h"

#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int width;
	int height;

	static bool haveFocus = true;

	switch (uMsg)
	{
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);

		g_WindowWidth = width;
		g_WindowHeight = height;

		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;



	case WM_KILLFOCUS:
		haveFocus = false;
		memset(g_Keyboard, false, 256 * sizeof(g_Keyboard[0]));
		break;

	case WM_SETFOCUS: haveFocus = true; break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (haveFocus)
		{
			bool keyIsDown = ((lParam & (1 << 31)) == 0);
			bool keyWasDown = ((lParam & (1 << 30)) != 0);
			if (keyIsDown != keyWasDown)
			{
				g_Keyboard[(uint8_t)wParam] = keyIsDown;
			}
		}
		break;


	default:
		break;
	}


	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

SWindow::SWindow()
	: m_hInstance(GetModuleHandle(nullptr))
{

	const wchar_t* CLASS_NAME = L"SWindow";

	WNDCLASS wndClass = {};
	// Window name, NOT TITEL
	wndClass.lpszClassName = CLASS_NAME;
	// class handel
	wndClass.hInstance = m_hInstance;
	// icon
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	// curser
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// window Procedure-function
	wndClass.lpfnWndProc = WindowProc;


	RegisterClass(&wndClass);

	// styles for the window
	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW;

	int width = g_WindowWidth;
	int height = g_WindowHeight;

	// window dimentions
	RECT rect;
	rect.left = 30;
	rect.top = 30;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	// adjust the proportions so the width and height are for the canvas not the outer borther
	AdjustWindowRect(&rect, style, false);
	
	m_hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Tttle",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		m_hInstance,
		NULL
	);

	ShowWindow(m_hWnd, SW_SHOW);

	m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth = g_WindowWidth;
	m_bmi.bmiHeader.biHeight = -g_WindowHeight; // top-down
	m_bmi.bmiHeader.biPlanes = 1;
	m_bmi.bmiHeader.biBitCount = 32;
	m_bmi.bmiHeader.biCompression = BI_RGB;

	m_hdc = GetDC(m_hWnd);
}

SWindow::~SWindow()
{
	const wchar_t* CLASS_NAME = L"SWindow";

	UnregisterClass(CLASS_NAME, m_hInstance);

	ReleaseDC(m_hWnd, m_hdc);
}

bool SWindow::ProcessMessages()
{

	MSG msg = {};

	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

bool SWindow::DrawBuffer(const uint8_t* bufferBGRA)
{

	m_bmi.bmiHeader.biWidth = g_WindowWidth;
	m_bmi.bmiHeader.biHeight = g_WindowHeight; // top-down

	// Draw buffer
	SetDIBitsToDevice(m_hdc, 0, 0, g_WindowWidth, g_WindowHeight, 0, 0, 0, g_WindowHeight, bufferBGRA, &m_bmi, DIB_RGB_COLORS);

	return false;
}

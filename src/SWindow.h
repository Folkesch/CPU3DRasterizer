#pragma once
#include <Windows.h>
#include <stdint.h>

inline bool g_Keyboard[256] = { 0 };

struct MouseEvent
{
	int32_t x, y;
	uint8_t Buttons;
};
enum { MOUSE_LEFT = 0b1, MOUSE_MIDDEL = 0b10, MOUSE_RIGHT = 0b100, MOUSE_X1 = 0b1000, MOUSE_X2 = 0b10000};



LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

inline uint32_t g_WindowWidth = 1200;
inline uint32_t g_WindowHeight = 1200;

class SWindow
{
public:
	SWindow();

    SWindow(SWindow&) = delete;

	~SWindow();

	bool ProcessMessages();

    bool DrawBuffer(const uint8_t* buffer);

private:
	HINSTANCE m_hInstance = nullptr;
	HWND m_hWnd = nullptr;

    BITMAPINFO m_bmi = {};
    HDC m_hdc = nullptr;
};
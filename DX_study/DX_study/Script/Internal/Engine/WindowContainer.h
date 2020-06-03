#pragma once
#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"

class WindowContainer {
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CloseWindow();
protected:
	RenderWindow m_RenderWindow;
	KeyboardClass m_Keyboard;
	MouseClass m_Mouse;
};
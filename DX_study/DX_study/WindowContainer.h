#pragma once
#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"
#include "Graphics/Graphics.h"

class WindowContainer {
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow m_RenderWindow;
	KeyboardClass m_Keyboard;
	MouseClass m_Mouse;
	KeyboardEvent m_KeyboardEvent;
	MouseEvent m_MouseEvent;
private:
	
};
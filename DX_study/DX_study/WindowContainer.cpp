#include "WindowContainer.h"

WindowContainer::WindowContainer() {
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false) {

		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
			ErrorLogger::Log(GetLastError(), "Failed to register raw input devices.");
			exit(-1);
		}
		raw_input_initialized = true;
	}
}

//ImGui 함수 가져오기 -> win32 연계
extern LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam);

LRESULT WindowContainer::WindowProc(
	HWND hwnd, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam) 
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) return false;

	switch (uMsg) {
		//keyboard message
	case WM_KEYDOWN: {
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (m_Keyboard.IsKeysAutoRepeat()) {
			m_Keyboard.OnKeyPressed(keycode);
		}
		else {
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed) {
				m_Keyboard.OnKeyPressed(keycode);
			}
		}
		return 0;
	}
	case WM_KEYUP: {
		unsigned char keycode = static_cast<unsigned char>(wParam);
		m_Keyboard.OnKeyReleased(keycode);
		return 0;
	}
	case WM_CHAR: {
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (m_Keyboard.IsCharsAutoRepeat()) {
			m_Keyboard.OnChar(ch);
		}
		else {
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed) {
				m_Keyboard.OnChar(ch);
			}
		}
		return 0;
	}
				  //Mouse Message
	case WM_MOUSEMOVE: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		m_Mouse.OnMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		m_Mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		m_Mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		m_Mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		m_Mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			m_Mouse.OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
			m_Mouse.OnWheelDown(x, y);
		}
		break;
	}
	case WM_INPUT: {//MouseMoveRaw 하면서 작성한 코드인데 진짜 이해안됨.
		UINT dataSize;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

		if (dataSize > 0) {
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize) {
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE) {
					m_Mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
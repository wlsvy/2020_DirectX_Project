#include "WindowContainer.h"

bool RenderWindow::Initialize(
	WindowContainer * pWindowContainer, 
	HINSTANCE hInstance, 
	std::string window_title, 
	std::string window_class, 
	int width, 
	int m_Height) 
{
	this->m_HInstance = hInstance;
	this->m_Width = width;
	this->m_Height = m_Height;
	this->m_WindowTitle = window_title;
	this->m_WideWindowTitle = StringHelper::StringToWide(this->m_WindowTitle);
	this->m_WindowClass = window_class;
	this->m_WideWindowClass = StringHelper::StringToWide(this->m_WindowClass);

	this->RegisterWindowClass();

	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->m_Width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->m_Height / 2;

	RECT wr; 
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + this->m_Width;
	wr.bottom = wr.top + this->m_Height;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE); 

	this->m_Handle = CreateWindowEx(
		0, 
		this->m_WideWindowClass.c_str(), 
		this->m_WideWindowTitle.c_str(), 
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 
		wr.left,
		wr.top, 
		wr.right - wr.left, 
		wr.bottom - wr.top, 
		NULL, 
		NULL, 
		this->m_HInstance,
		pWindowContainer); 

	if (this->m_Handle == NULL) {
		ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->m_WindowTitle);
		return false;
	}

	ShowWindow(this->m_Handle, SW_SHOW);
	SetForegroundWindow(this->m_Handle);
	SetFocus(this->m_Handle);

	return true;
}

RenderWindow::~RenderWindow() {
	if (this->m_Handle != NULL) {
		UnregisterClass(this->m_WideWindowClass.c_str(), this->m_HInstance);
		DestroyWindow(m_Handle);
	}
}

LRESULT CALLBACK HandleMsgRedirect(
	HWND hwnd, 
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam) 
{
	switch (uMsg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
	{
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(
	HWND hwnd,
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam) 
{
	switch (uMsg) {
	case WM_NCCREATE: {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer * pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) 
		{
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	}
	
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup; 
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->m_HInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->m_WideWindowClass.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
}

bool RenderWindow::ProcessMessage() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(
		&msg,
		this->m_Handle, 
		0,
		0,
		PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL) {
		if (!IsWindow(this->m_Handle)) {
			this->m_Handle = NULL;
			UnregisterClass(this->m_WideWindowClass.c_str(), this->m_HInstance);
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const
{
	return this->m_Handle;
}

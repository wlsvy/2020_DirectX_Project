#include "WindowContainer.h"

bool RenderWindow::Initialize(WindowContainer * pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) {
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->window_title = window_title;
	this->window_title_wide = StringHelper::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringHelper::StringToWide(this->window_class);

	//������ â ũ��� ���� Ÿ��Ʋ �ٱ��� �����ؼ� ������. �� Ÿ��Ʋ �ٸ� ������ ũ���� ������ ����.
	this->RegisterWindowClass();

	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;

	RECT wr; // Window rectangle
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + this->width;
	wr.bottom = wr.top + this->height;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE); // ������ ũ�� ����

	this->handle = CreateWindowEx(0, //Extended Window style -> 0�� ����Ʈ
		this->window_class_wide.c_str(), //Ŭ���� �̸�
		this->window_title_wide.c_str(), //Ÿ��Ʋ �̸�
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //������ ��Ÿ��
		wr.left, //������ x ��ġ
		wr.top, //������ y ��ġ
		wr.right - wr.left, //â �ʺ�
		wr.bottom - wr.top, //â ����
		NULL, //������ parent, ���� parent window ����
		NULL, //chile window Ȥ�� menu, ���� child window ����
		this->hInstance,//������ instance
		pWindowContainer); //param to create window

	if (this->handle == NULL) {
		ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->window_title);
		return false;
	}

	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

RenderWindow::~RenderWindow() {
	if (this->handle != NULL) {
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		//all other messages
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
	{
		//retrieve ptr to window class
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		//forward message to window class handler
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) { //�޼����� ������ �پ��ؼ� ����ġ Ȱ��. ���� ��� ->Ű�� �������� ��
	case WM_NCCREATE: {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer * pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) //Sanity check
		{
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam); //�⺻ ������ ���μ��� �¾�
	}
	
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX wc; //������ Ŭ���� Ȯ�� ����
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup; //�̰� �ǳ�?? �Ķ���� �� �ٿ��µ�
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_wide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
}

bool RenderWindow::ProcessMessage() {
	//Handling the Window Message
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));//�޼��� ���� �ʱ�ȭ

	while (PeekMessage(&msg,//�޼��� ���� ���
		this->handle, //handle to window we are checking messages for
		0, //Minimum Filter Msg Value - We are not filtering messages
		0, //Maximm Filter Msg value
		PM_REMOVE)) //Remove Message after capturing if via peekMessage
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//â�� �������� Ȯ��
	if (msg.message == WM_NULL) {
		if (!IsWindow(this->handle)) {
			this->handle = NULL; //Message Processing loop takes care of destroying this window
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const
{
	return this->handle;
}

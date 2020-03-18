#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	bool Initialize(WindowContainer * pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessage();
	HWND GetHWND() const;
	~RenderWindow();
private:
	void RegisterWindowClass(); //������ ���� �� Ŭ���� �̸� �ο��ϰ� ��ϵ� �ؾ���.
	HWND handle = NULL; //Handle to this window
	HINSTANCE hInstance = NULL;  //Handle to application Instance
	std::string window_title = "";
	std::wstring window_title_wide = L"";// Wstring ������ ���� repsresentation
	std::string window_class = "";
	std::wstring window_class_wide = L""; // ������ Ŭ���� �̸� wstring
	int width = 0;
	int height = 0;
};
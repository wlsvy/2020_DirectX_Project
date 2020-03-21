#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	bool Initialize(
		WindowContainer * pWindowContainer, 
		HINSTANCE hInstance, 
		std::string window_title, 
		std::string window_class, 
		int width, 
		int height);
	bool ProcessMessage();
	HWND GetHWND() const;
	~RenderWindow();
private:
	void RegisterWindowClass(); 

	HWND m_Handle = NULL;
	HINSTANCE m_HInstance = NULL; 
	std::string m_WindowTitle = "";
	std::wstring m_WideWindowTitle = L"";
	std::string m_WindowClass = "";
	std::wstring m_WideWindowClass = L"";
	int m_Width = 0;
	int m_Height = 0;
};
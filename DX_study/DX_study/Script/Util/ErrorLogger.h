#pragma once
#include "COMException.h"
#include <Windows.h>


namespace StringHelper {
	void ErrorLog(const char * message);
	void ErrorLog(const std::string & message);
	void ErrorLog(HRESULT hr, std::string message);
	void ErrorLog(HRESULT hr, std::wstring message);
	void ErrorLog(CustomException & exception);
}
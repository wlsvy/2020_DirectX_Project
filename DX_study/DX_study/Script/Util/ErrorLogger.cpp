#include "ErrorLogger.h"
#include <comdef.h>

void StringHelper::ErrorLog(const char * message)
{
	MessageBoxA(NULL, message, "Error", MB_ICONERROR);
}

void StringHelper::ErrorLog(const std::string & message) {
	std::string error_message = "Error : " + message;
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void StringHelper::ErrorLog(HRESULT hr, std::string message) {
	_com_error error(hr);
	std::wstring error_message = L"Error: " + StringHelper::StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void StringHelper::ErrorLog(HRESULT hr, std::wstring message) {
	_com_error error(hr);
	std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void StringHelper::ErrorLog(CustomException & exception)
{
	std::wstring error_message = exception.what();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

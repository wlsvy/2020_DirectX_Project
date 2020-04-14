#pragma once
#include <comdef.h>
#include <stdexcept>
#include "StringHelper.h"

class CustomException {
public:
	CustomException(HRESULT hr, const std::string& msg) {
		_com_error error(hr);
		whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg)) + L"\n";
		whatmsg += error.ErrorMessage();
	}
	CustomException(const std::string& msg) {
		whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg)) + L"\n";
	}

	const wchar_t * what() const {
		return whatmsg.c_str();
	}
private:
	std::wstring whatmsg;
};

inline void ThrowIfFailed(HRESULT hr, const std::string& msg)
{
	if (FAILED(hr))
	{
		throw CustomException(hr, msg);
	}
}

inline void ThrowIfFailed(bool result, const std::string& msg)
{
	if (result == false)
	{
		throw CustomException(msg);
	}
}
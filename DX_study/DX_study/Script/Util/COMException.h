#pragma once
#include <comdef.h>
#include <stdexcept>
#include "StringHelper.h"

class COMException {
public:
	COMException(HRESULT hr, const std::string& msg) {
		_com_error error(hr);
		whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg)) + L"\n";
		whatmsg += error.ErrorMessage();
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
		throw COMException(hr, msg);
	}
}
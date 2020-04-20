#pragma once
#include <string>

class StringHelper {
public:
	static std::wstring StringToWide(std::string str);
	static std::string GetDirectoryFromPath(const std::string & filepath);
	static std::string GetFileNameFromPath(const std::string & filepath, bool removeExtension = true);
	static std::string GetFileExtension(const std::string & filename);
	static std::string EraseFileExtension(const std::string & filePath);
	static std::string GetUpperDirectroyName(const std::string & filePath);
};

#include <DirectXMath.h>
namespace std {
	_NODISCARD inline string to_string(const DirectX::XMVECTOR & val)
	{
		return "(" +
			std::to_string(val.m128_f32[0]) + ", " +
			std::to_string(val.m128_f32[1]) + ", " +
			std::to_string(val.m128_f32[2]) + ", " +
			std::to_string(val.m128_f32[3]) +
			")";
	}
	_NODISCARD inline string to_string(const DirectX::XMFLOAT4 & val)
	{
		return "(" +
			std::to_string(val.x) + ", " +
			std::to_string(val.y) + ", " +
			std::to_string(val.z) + ", " +
			std::to_string(val.w) +
			")";
	}
	_NODISCARD inline string to_string(const DirectX::XMFLOAT3 & val)
	{
		return "(" +
			std::to_string(val.x) + ", " +
			std::to_string(val.y) + ", " +
			std::to_string(val.z) +
			")";
	}
	_NODISCARD inline string to_string(const DirectX::XMMATRIX & val)
	{
		return
			to_string(val.r[0]) + "\n" +
			to_string(val.r[1]) + "\n" +
			to_string(val.r[2]) + "\n" +
			to_string(val.r[3]);
	}
}
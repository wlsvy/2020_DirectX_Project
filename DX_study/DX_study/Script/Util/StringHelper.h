#pragma once
#include <string>

class StringHelper {
public:
	static std::wstring StringToWide(std::string str);
	static std::string GetDirectoryFromPath(const std::string & filepath);
	static std::string GetNameFromPath(const std::string & filepath, bool removeExtension = true);
	static std::string GetFileExtension(const std::string & filename);
	static std::string EraseFileExtension(const std::string & filePath);
};
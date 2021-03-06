#include "StringHelper.h"
#include <algorithm>

std::wstring StringHelper::StringToWide(std::string str) {
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

std::string StringHelper::GetDirectoryFromPath(const std::string & filepath)
{
	//경로명의 \\ 혹은 / 확인
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) {
		return "";
	}
	if (off1 == std::string::npos) {
		return filepath.substr(0, off2);
	}
	if (off2 == std::string::npos) {
		return filepath.substr(0, off1);
	}
	return filepath.substr(0, std::max(off1, off2)); //둘 다 존재하면 높은 값으로
}

std::string StringHelper::GetFileNameFromPath(const std::string & filepath, bool removeExtension)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) {
		if (removeExtension) {
			size_t extOff = filepath.find_last_of('.');
			return filepath.substr(0, extOff);
		}
		return filepath;
	}

	size_t off;
	if (off1 == std::string::npos) { off = off2; }
	else if (off2 == std::string::npos) { off = off1; }
	else { off = std::max(off1, off2); }

	if (removeExtension) {
		size_t extOff = filepath.find_last_of('.');
		return filepath.substr(off + 1, extOff - off - 1);
	}
	return filepath.substr(off + 1, filepath.size() - off - 1);
}

std::string StringHelper::GetFileExtension(const std::string & filename)
{
	size_t off = filename.find_last_of('.');
	if (off == std::string::npos) {
		return {};
	}

	return filename.substr(off + 1);//확장자 빼기. ex : file.png 에서 . 위치 확인하고 그 뒤로는 제거
}

std::string StringHelper::EraseFileExtension(const std::string & filePath)
{
	size_t off = filePath.find_last_of('.');
	if (off == std::string::npos) {
		return filePath;
	}

	return filePath.substr(0, off);
}

std::string StringHelper::GetUpperDirectroyName(const std::string & filePath)
{
	size_t off1 = filePath.find_last_of('\\');
	size_t off2 = filePath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) return "";

	size_t end;
	if (off1 == std::string::npos) { end = off2; }
	else if (off2 == std::string::npos) { end = off1; }
	else { end = std::max(off1, off2); }

	off1 = filePath.find_last_of('\\', std::max(end - 1, (size_t)0));
	off2 = filePath.find_last_of('/', std::max(end - 1, (size_t)0));

	size_t start;
	if (off1 == std::string::npos && off2 == std::string::npos) { start = 0; }
	else if (off1 == std::string::npos) { start = off2 + 1; }
	else if (off2 == std::string::npos) { start = off1 + 1; }
	else { start = std::max(off1, off2) + 1; }

	return filePath.substr(start, end - start);
}

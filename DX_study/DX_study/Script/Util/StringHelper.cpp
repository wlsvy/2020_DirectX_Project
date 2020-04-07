#include "StringHelper.h"
#include <algorithm>

std::wstring StringHelper::StringToWide(std::string str) {
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

std::string StringHelper::GetDirectoryFromPath(const std::string & filepath)
{
	//��θ��� \\ Ȥ�� / Ȯ��
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
	return filepath.substr(0, std::max(off1, off2)); //�� �� �����ϸ� ���� ������
}

std::string StringHelper::GetNameFromPath(const std::string & filepath, bool removeExtension)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) return "";

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

	return std::string(filename.substr(off + 1));//Ȯ���� ����. ex : file.png ���� . ��ġ Ȯ���ϰ� �� �ڷδ� ����
}

std::string StringHelper::EraseFileExtension(const std::string & filePath)
{
	size_t off = filePath.find_last_of('.');
	if (off == std::string::npos) {
		return {};
	}

	return std::string(filePath.substr(0, off));
}

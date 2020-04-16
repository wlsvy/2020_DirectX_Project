#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace Importer {
	void LoadData();
	void LoadModel(const std::string & dirPath, const std::string& name);
	void LoadAnimation(const std::string & dirPath, const std::string& name);
	void LoadVertexShader(const std::string & dirPath, const std::string& name);
	void LoadVertexShader2D(const std::string & dirPath, const std::string& name);
	void LoadVertexShader3D(const std::string & dirPath, const std::string& name);
	void LoadPixelShader(const std::string & dirPath, const std::string& name);
	void LoadTexture(const std::string & dirPath, const std::string& name);
	void LoadCSV(const std::string & dirPath, const std::string& name);
	std::unordered_map<std::string, std::vector<std::string>> LoadCSV(const std::string & filePath);
	

	void TraverseDirectory(
		const std::string & dirPath, 
		void(*callBack)(const std::string &, const std::string &));
	
}
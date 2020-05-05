#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace Importer {
	bool LoadBaseResources();
	bool LoadModelResources();
	void LoadModel(const std::string & dirPath, const std::string& name);
	void LoadAnimation(const std::string & dirPath, const std::string& name);
	void LoadVertexShader(const std::string & dirPath, const std::string& name);
	void LoadVertexShader2D(const std::string & dirPath, const std::string& name);
	void LoadVertexShader3D(const std::string & dirPath, const std::string& name);
	void LoadPixelShader(const std::string & dirPath, const std::string& name);
	void LoadComputeShader(const std::string & dirPath, const std::string& name);
	void LoadGeometryShader(const std::string & dirPath, const std::string& name);
	void LoadTexture(const std::string & dirPath, const std::string& name);
	std::unordered_map<std::string, std::vector<std::string>> LoadCSV(const std::string & filePath);
	std::vector<std::string> SplitString(const std::string & str, const char delimiter);
	

	void TraverseDirectory(
		const std::string & dirPath, 
		void(*callBack)(const std::string &, const std::string &));
	
}
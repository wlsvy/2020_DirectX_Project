#include "ImportHelper.h"
#include "FileImporter.h"

#include "../Graphics/Texture.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../../Util/ErrorLogger.h"
#include "ObjectPool.h"
#include <io.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>

bool Importer::LoadBaseResources()
{
	try {
		TraverseDirectory("hlsl/VertexShader/2D/", &Importer::LoadVertexShader2D);
		TraverseDirectory("hlsl/VertexShader/3D/", &Importer::LoadVertexShader3D);
		TraverseDirectory("hlsl/VertexShader/3D_Skinned/", &Importer::LoadVertexShader);
		TraverseDirectory("hlsl/PixelShader/", &Importer::LoadPixelShader);
		TraverseDirectory("hlsl/ComputeShader/", &Importer::LoadComputeShader);
		TraverseDirectory("hlsl/GeometryShader/", &Importer::LoadGeometryShader);
		TraverseDirectory("Data/Textures/", &Importer::LoadTexture);
		return true;
	}
	catch (std::exception & e) {
		StringHelper::ErrorLog(e.what());
		return false;
	}
}

bool Importer::LoadModelResources()
{
	try {
		TraverseDirectory("Data/Objects/", &Importer::LoadModel);
		TraverseDirectory("Data/Animation/", &Importer::LoadAnimation);
		return true;
	}
	catch (std::exception & e) {
		e.what();
		return false;
	}
}

bool CreateModel(const std::string & dirPath, const std::string& name) {
	auto filePath = dirPath + name;

	const auto importer_flags =
		aiProcess_MakeLeftHanded |              // directx style.
		aiProcess_FlipUVs |                     // directx style.
		aiProcess_FlipWindingOrder |            // directx style.
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |              // reduce the number of meshes         
		aiProcess_ImproveCacheLocality |        // re-order triangles for better vertex cache locality.
		aiProcess_RemoveRedundantMaterials |    // remove redundant/unreferenced materials.
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |                 // splits meshes with more than one primitive type in homogeneous sub-meshes.
		aiProcess_FindDegenerates |             // convert degenerate primitives to proper lines or points.
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure;

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(
		filePath,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr) return false;

	if (pScene->HasMeshes()) {
		if (!pScene->HasAnimations()) {
			ModelImporter importer;
			return importer.CreateModel(dirPath, StringHelper::EraseFileExtension(name), pScene);
		}
		else {
			SkinnedModelImporter importer;
			return importer.CreateModel(dirPath, StringHelper::EraseFileExtension(name), pScene);
		}
	}

	return false;
}
void Importer::LoadModel(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "fbx" ||
		ext == "obj")
	{
		if (!CreateModel(dirPath, name)) {
			MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
			return;
		}
	}
	
}

bool CreateAnimation(const std::string & dirPath, const std::string& name) {
	auto filePath = dirPath + name;
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr) return false;

	if (pScene->HasAnimations()) {
		AnimationImporter animImporter;
		auto avatarName = StringHelper::GetUpperDirectroyName(filePath);

		animImporter.LoadAnimation(
			avatarName + "_" + StringHelper::EraseFileExtension(name),
			Core::Find<SkinnedModel>(avatarName), pScene);
	}

	return true;
}
void Importer::LoadAnimation(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "fbx")
	{
		if (!CreateAnimation(dirPath, name)) {
			MessageBoxA(NULL, "Animation Initialize error.", ERROR, MB_ICONERROR);
			return;
		}
	}
	
}

void Importer::LoadVertexShader2D(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Core::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetShaderCsoFilePath() + StringHelper::EraseFileExtension(name) + ".cso",
			layout,
			ARRAYSIZE(layout)))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadVertexShader3D(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Core::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetShaderCsoFilePath() + StringHelper::EraseFileExtension(name) + ".cso",
			layout,
			ARRAYSIZE(layout)))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadVertexShader(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEID", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEWEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Core::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetShaderCsoFilePath() + StringHelper::EraseFileExtension(name) + ".cso",
			layout,
			ARRAYSIZE(layout)))
		{
			Core::Destroy(vs);
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadPixelShader(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		auto ps = Core::CreateInstance<PixelShader>();
		if (!ps->Initialize(Core::GetShaderCsoFilePath() + StringHelper::EraseFileExtension(name) + ".cso"))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadComputeShader(const std::string & dirPath, const std::string & name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		auto cs = Core::CreateInstance<ComputeShader>();
		if (!cs->Initialize(Core::GetShaderCsoFilePath() + StringHelper::EraseFileExtension(name) + ".cso"))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadGeometryShader(const std::string & dirPath, const std::string & name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		auto gs = Core::CreateInstance<GeometryShader>();
		if (!gs->Initialize(Core::GetShaderCsoFilePath() + StringHelper::EraseFileExtension(name) + ".cso"))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadTexture(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "jpg" ||
		ext == "png" ||
		ext == "bmp" ||
		ext == "dds")
	{
		Core::CreateInstance<Texture>(dirPath + name);
	}
}

std::unordered_map<std::string, std::vector<std::string>> Importer::LoadCSV(const std::string & filePath)
{
	CsvImporter importer;
	return importer.LoadCSV(filePath);
}

std::vector<std::string> Importer::SplitString(const std::string & str, char delimiter)
{
	std::vector<std::string> vs;
	std::string word;

	std::stringstream stream(str);

	while (std::getline(stream, word, delimiter)) vs.push_back(word);
	return vs;
}

void Importer::TraverseDirectory(const std::string & dirPath, void(*callBack)(const std::string &, const std::string &))
{
	struct _finddata_t fd;
	intptr_t handle;

	std::string filePath = dirPath + "*.*";

	if ((handle = _findfirst(filePath.c_str(), &fd)) != -1L) {
		do {
			if (fd.attrib & _A_SUBDIR &&
				(fd.name != std::string(".")) &&
				(fd.name != std::string("..")))
			{
				std::string subDirPath = dirPath + fd.name + "/";
				TraverseDirectory(subDirPath, callBack);
				continue;
			}

			(*callBack)(dirPath, fd.name);
		} while (_findnext(handle, &fd) == 0);
	}
	_findclose(handle);
}

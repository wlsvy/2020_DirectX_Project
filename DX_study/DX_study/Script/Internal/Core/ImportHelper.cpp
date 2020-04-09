#include "ImportHelper.h"
#include "FileImporter.h"

#include "../Graphics/Texture.h"
#include "../../Util/ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <DirectXMath.h>

bool Importer::LoadModel(const std::string & filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	if (!pScene->HasAnimations()) {
		ModelImporter importer;
		return importer.LoadModel(filePath, pScene);
	}
	else {
		SkinnedModelImporter importer;
		return importer.LoadModel(filePath, pScene);
	}

	return true;
}
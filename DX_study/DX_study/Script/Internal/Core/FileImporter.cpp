#include "FileImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>

#include "ObjectPool.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Model.h"
#include "../Graphics/AnimationClip.h"
#include "../Graphics/Shaders.h"
#include "../../Util/StringHelper.h"
#include "../../Util/Math.h"

bool ModelImporter::CreateModel(
	const std::string & dirPath,
	const std::string & fileName,
	const aiScene * scene)
{
	m_Directory = dirPath;

	this->ProcessNode(scene->mRootNode, scene, DirectX::XMMatrixIdentity());

	auto model = Core::CreateInstance<Model>(m_Renderable, StringHelper::GetFileNameFromPath(fileName));
	return true;
}

void ModelImporterBase::ProcessNode(aiNode * node, const aiScene * scene, const DirectX::XMMATRIX & parentTransformMatrix)
{
	DirectX::XMMATRIX nodeTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, nodeTransformMatrix);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

void ModelImporter::ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix)
{
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;
	
	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTangents) {
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;
		}
		
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	auto newMesh = Core::CreateInstance<MeshReal<Vertex3D>>(vertices, indices, transformMatrix, mesh->mName.data);

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	auto mat = LoadMaterial(material, scene);

	m_Renderable.emplace_back(newMesh, mat, ShaderState::GetDefault());
}

TextureStorageType ModelImporterBase::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

std::shared_ptr<Texture> ModelImporterBase::LoadTexture(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene) {
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) return std::shared_ptr<Texture>();

	aiString path;
	pMaterial->GetTexture(textureType, 0, &path);
	TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, 0, textureType);
	std::shared_ptr<Texture> texturePtr;
	switch (storetype)
	{
		case TextureStorageType::EmbeddedIndexCompressed:
		{
			int index = GetTextureIndex(&path);
			texturePtr = Core::CreateInstance<Texture>(
				reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
				pScene->mTextures[index]->mWidth);
			texturePtr->Name = pScene->mTextures[index]->mFilename.data;
			break;
		}
		case TextureStorageType::EmbeddedCompressed:
		{
			const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
			texturePtr = Core::CreateInstance<Texture>(
				reinterpret_cast<uint8_t*>(pTexture->pcData),
				pTexture->mWidth);
			texturePtr->Name = pTexture->mFilename.data;
			break;
		}
		case TextureStorageType::Disk:
		{
			std::string filename = m_Directory + '\\' + path.C_Str();
			texturePtr = Core::CreateInstance<Texture>(filename);
			texturePtr->Name = StringHelper::GetFileNameFromPath(filename);
			break;
		}
	}
	return texturePtr;
}
std::shared_ptr<SharedMaterial> ModelImporterBase::LoadMaterial(aiMaterial * pMaterial, const aiScene * pScene)
{
	TextureStorageType storetype = TextureStorageType::Invalid;

	aiString matName;
	pMaterial->Get(AI_MATKEY_NAME, matName);

	auto mat = Core::CreateInstance<SharedMaterial>(matName.data);

	if (auto albedo = LoadTexture(pMaterial, aiTextureType::aiTextureType_DIFFUSE, pScene)) {
		mat->Albedo = albedo;
	}
	else {
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
		mat->Albedo = Texture::GetDefault();
		mat->Color = DirectX::XMFLOAT4(aiColor.r, aiColor.g, aiColor.b, 1.0f);
	}
	if (auto normal = LoadTexture(pMaterial, aiTextureType::aiTextureType_NORMALS, pScene)) {
		mat->Normal = normal;
	}
	if (auto metal = LoadTexture(pMaterial, aiTextureType::aiTextureType_REFLECTION, pScene)) {
		mat->Metal = metal;
	}
	if (auto roughness = LoadTexture(pMaterial, aiTextureType::aiTextureType_SHININESS, pScene)) {
		mat->Roughness = roughness;
	}
	if (auto emission = LoadTexture(pMaterial, aiTextureType::aiTextureType_EMISSIVE, pScene)) {
		mat->Emission = emission;
	}

	{
		pMaterial->Get(AI_MATKEY_REFLECTIVITY, mat->MetalIntensity);
		pMaterial->Get(AI_MATKEY_SHININESS, mat->RoughnessIntensity);
		mat->MetalIntensity *= 0.01f;
		mat->RoughnessIntensity *= 0.01f;

		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor);
		mat->EmissionColor = DirectX::XMFLOAT3(aiColor.r, aiColor.g, aiColor.b);
	}

	return mat;
}

int ModelImporterBase::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

bool SkinnedModelImporter::CreateModel(
	const std::string & dirPath,
	const std::string & fileName,
	const aiScene * scene)
{
	m_Directory = dirPath;
	
	this->ProcessNode(scene->mRootNode, scene, DirectX::XMMatrixIdentity());

	auto model = Core::CreateInstance<SkinnedModel>(m_Renderable, m_BoneOffsets, m_BoneIdMap, fileName);
	return true;
}

DirectX::XMMATRIX GetAiMatrixData(aiMatrix4x4 & pSource);

void SkinnedModelImporter::ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix)
{
	// Data to fill
	const UINT vericesSize = mesh->mNumVertices;

	std::vector<DWORD> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<Vertex_BoneData> bones;
	std::vector<SkinnedVertex> vertices(vericesSize);

	bones.resize(mesh->mNumVertices);

	//Get Bones
	USHORT boneIndex = 0;

	for (USHORT i = 0; i < mesh->mNumBones; i++) {
		aiBone *bone = mesh->mBones[i];
		std::string boneName = bone->mName.data;

		if (m_BoneIdMap.find(boneName) == m_BoneIdMap.end()) {
			boneIndex = (USHORT)m_BoneIdMap.size();
			m_BoneOffsets.push_back(DirectX::XMMatrixTranspose(GetAiMatrixData(bone->mOffsetMatrix)));
			m_BoneIdMap[boneName] = boneIndex;
		}
		else {
			boneIndex = m_BoneIdMap[boneName];
		}

		for (USHORT j = 0; j < bone->mNumWeights; j++) {
			USHORT VertexID = bone->mWeights[j].mVertexId;
			float Weight = bone->mWeights[j].mWeight;

			for (USHORT index = 0; index < Vertex_BoneData::MAX_BONE_PER_VERTEX; index++) {
				if (bones[VertexID].BoneIDs[index] == -1)
				{
					bones[VertexID].BoneIDs[index] = boneIndex;
					bones[VertexID].BoneWeights[index] = Weight;
					break;
				}
			}
		}
	}

	//Get vertices
	for (UINT i = 0; i < vericesSize; i++)
	{
		vertices[i].pos.x = mesh->mVertices[i].x;
		vertices[i].pos.y = mesh->mVertices[i].y;
		vertices[i].pos.z = mesh->mVertices[i].z;
		vertices[i].normal.x = mesh->mNormals[i].x;
		vertices[i].normal.y = mesh->mNormals[i].y;
		vertices[i].normal.z = mesh->mNormals[i].z;
		vertices[i].tangent.x = mesh->mTangents[i].x;
		vertices[i].tangent.y = mesh->mTangents[i].y;
		vertices[i].tangent.z = mesh->mTangents[i].z;


		if (mesh->mTextureCoords[0])
		{
			vertices[i].texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertices[i].texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices[i].boneIDs = DirectX::XMINT4(
			bones[i].BoneIDs[0], bones[i].BoneIDs[1],
			bones[i].BoneIDs[2], bones[i].BoneIDs[3]);
		vertices[i].boneWeights = DirectX::XMFLOAT4(
			bones[i].BoneWeights[0], bones[i].BoneWeights[1],
			bones[i].BoneWeights[2], bones[i].BoneWeights[3]);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	auto newMesh = Core::CreateInstance<MeshReal<SkinnedVertex>>(vertices, indices, transformMatrix, mesh->mName.data);

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	auto mat = LoadMaterial(material, scene);

	m_Renderable.emplace_back(newMesh, mat, ShaderState::GetSkinnedDefault());
}

DirectX::XMMATRIX GetAiMatrixData(aiMatrix4x4 & pSource)
{
	return DirectX::XMMatrixSet(
		pSource.a1, pSource.a2, pSource.a3, pSource.a4,
		pSource.b1, pSource.b2, pSource.b3, pSource.b4,
		pSource.c1, pSource.c2, pSource.c3, pSource.c4,
		pSource.d1, pSource.d2, pSource.d3, pSource.d4);
}

void AnimationImporter::LoadAnimation(const std::string & name, const std::shared_ptr<SkinnedModel> & baseModel, const aiScene * scene)
{
	m_BaseModel = baseModel;

	USHORT animationNum = scene->mNumAnimations;
	for (USHORT i = 0; i < animationNum; i++) {
		aiAnimation *anim = scene->mAnimations[i];
		ProcessAnimation(name, anim, scene);
	}

}

void AnimationImporter::ProcessAnimation(const std::string & name, aiAnimation * anim, const aiScene * scene)
{
	auto& boneIdMap = m_BaseModel->GetBoneIdMap();
	auto& boneOffsets = m_BaseModel->GetBoneOffsets();

	auto clip = Core::CreateInstance<AnimationClip>();
	clip->Name = name;
	clip->Channels.resize(boneIdMap.size());
	clip->TickPerSecond = (float)(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);
	clip->Duration = (float)anim->mDuration;
	clip->Avatar = m_BaseModel;
	for (auto & p : boneIdMap) {
		auto & channel = clip->Channels[p.second];
		channel.ChannelName = p.first;
	}

	USHORT numChannel = anim->mNumChannels;
	for (USHORT i = 0; i < numChannel; i++) {
		aiNodeAnim *ainodeAnim = anim->mChannels[i];
		

		std::string nodeName = ainodeAnim->mNodeName.data;
		size_t pos = nodeName.find("_$Assimp");	//불필요한 문장 제거
		if (pos != std::string::npos)
		{
			nodeName = nodeName.substr(0, pos);
		}
		auto it = boneIdMap.find(nodeName);
		if (it == boneIdMap.end()) {
			continue;
		}

		USHORT boneIndex = it->second;
		BoneChannel & channel = clip->Channels[boneIndex];
		channel.NumPositionKeys = ainodeAnim->mNumPositionKeys;
		channel.NumRotationKeys = ainodeAnim->mNumRotationKeys;
		channel.NumScaleKeys = ainodeAnim->mNumScalingKeys;

		for (USHORT j = 0; j < channel.NumPositionKeys; j++) {
			auto& posKey = ainodeAnim->mPositionKeys[j];
			auto& posValue = posKey.mValue;

			channel.PositionKeys.emplace_back(posValue.x, posValue.y, posValue.z, posKey.mTime);
		}

		for (USHORT j = 0; j < channel.NumRotationKeys; j++) {
			auto& rotKey = ainodeAnim->mRotationKeys[j];
			auto& rotValue = rotKey.mValue;

			channel.RotationKeys.emplace_back(rotValue.x, rotValue.y, rotValue.z, rotValue.w, rotKey.mTime);
		}

		for (USHORT j = 0; j < channel.NumScaleKeys; j++) {
			auto& scaleKey = ainodeAnim->mScalingKeys[j];
			auto& scaleValue = scaleKey.mValue;

			channel.ScaleKeys.emplace_back(scaleValue.x, scaleValue.y, scaleValue.z, scaleKey.mTime);
		}
	}

	aiNode * ainode = scene->mRootNode;

	ProcessBoneHierarchy(ainode, clip.get(), nullptr, DirectX::XMMatrixIdentity());

	clip->NumChannel = (USHORT)clip->Channels.size();
	for (auto& channel : clip->Channels) {
		channel.NumChildBone = (USHORT)channel.ChildBoneIndex.size();
	}
}

void AnimationImporter::ProcessBoneHierarchy(aiNode * node, AnimationClip * animClip, BoneChannel * parentBone, const DirectX::XMMATRIX & parentTransform)
{
	auto& boneIdMap = m_BaseModel->GetBoneIdMap();
	std::string nodeName = node->mName.data;
	DirectX::XMMATRIX nodeTransform = GetAiMatrixData(node->mTransformation);
	nodeTransform = DirectX::XMMatrixTranspose(nodeTransform);
	DirectX::XMMATRIX globalTransform = nodeTransform * parentTransform;

	BoneChannel * currentBoneChannel = nullptr;

	auto it = boneIdMap.find(nodeName);
	if (it != boneIdMap.end()) {
		USHORT boneIndex = it->second;
		currentBoneChannel = &animClip->Channels[boneIndex];

		if (parentBone != nullptr) {
			parentBone->ChildBoneIndex.push_back(boneIndex);
		}
	}
	else {
		currentBoneChannel = parentBone;
	}

	USHORT childNum = node->mNumChildren;
	for (USHORT i = 0; i < childNum; i++) {
		aiNode * childNode = node->mChildren[i];
		ProcessBoneHierarchy(childNode, animClip, currentBoneChannel, globalTransform);
	}
}

CsvImporter::ColumnMap CsvImporter::LoadCSV(const std::string & filePath)
{
	std::ifstream fin;
	fin.open(filePath, std::ios::in);

	ColumnMap map;
	std::string line, word;
	int columnNum = 0;
	std::unordered_map<int, std::vector<std::string>*> columnIndexMap;

	fin >> line;
	std::stringstream s(line);
	while (getline(s, word, ','))
	{
		map.insert(std::make_pair(word, std::vector<std::string>()));
		columnIndexMap.insert(make_pair(columnNum++, &map[word]));
	}

	while (fin >> line) {
		columnNum = 0;

		std::stringstream s(line);
		while (getline(s, word, ','))
		{
			auto& colVector = *columnIndexMap[columnNum++];
			colVector.push_back(word);
		}
	}

	fin.close();
	return map;
}

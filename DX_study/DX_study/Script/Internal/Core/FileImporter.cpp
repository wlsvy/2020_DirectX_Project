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
#include "../../Util/StringHelper.h"


//int GetTextureIndex(aiString * pStr);
//TextureStorageType DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType);
//Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix);

bool ModelImporter::CreateModel(
	const std::string & dirPath,
	const std::string & fileName,
	const aiScene * scene)
{
	m_Directory = dirPath;

	this->ProcessNode(scene->mRootNode, scene, DirectX::XMMatrixIdentity());

	auto model = Pool::CreateInstance<Model>();
	model->Name = StringHelper::GetFileNameFromPath(fileName);
	if (!model->Initialize(m_Meshes)) {
		Pool::Destroy(model.get());
		return false;
	}
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

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	m_Meshes.emplace_back(vertices, indices, textures, transformMatrix);
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

std::vector<Texture> ModelImporterBase::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(
					reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
					pScene->mTextures[index]->mWidth,
					textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(
					reinterpret_cast<uint8_t*>(pTexture->pcData),
					pTexture->mWidth,
					textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = m_Directory + '\\' + path.C_Str();
				Texture diskTexture(filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}

	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;

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

	auto model = Pool::CreateInstance<SkinnedModel>();
	model->Name = fileName;
	if (!model->Initialize(m_Meshes, m_BoneOffsets, m_BoneIdMap)) {
		Pool::Destroy(model.get());
		return false;
	}

	return true;
}

DirectX::XMMATRIX GetAiMatrixData(aiMatrix4x4 & pSource);

void SkinnedModelImporter::ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix)
{
	// Data to fill
	const int vericesSize = mesh->mNumVertices;

	std::vector<DWORD> indices;
	std::vector<Texture> textures;
	std::vector<Vertex_Bone_Data> bones;
	std::vector<SkinnedVertex> vertices(vericesSize);

	bones.resize(mesh->mNumVertices);

	//Get Bones
	UINT BoneIndex = 0;

	for (UINT i = 0; i < mesh->mNumBones; i++) {
		aiBone *bone = mesh->mBones[i];
		std::string boneName = bone->mName.data;

		if (m_BoneIdMap.find(boneName) == m_BoneIdMap.end()) {
			BoneIndex = m_BoneIdMap.size();
			m_BoneOffsets.push_back(DirectX::XMMatrixTranspose(GetAiMatrixData(bone->mOffsetMatrix)));
			m_BoneIdMap[boneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneIdMap[boneName];
		}

		for (UINT j = 0; j < bone->mNumWeights; j++) {
			UINT VertexID = bone->mWeights[j].mVertexId;
			float Weight = bone->mWeights[j].mWeight;

			for (int index = 0; index < Vertex_Bone_Data::MAX_BONE_PER_VERTEX; index++) {
				if (bones[VertexID].BoneIDs[index] == -1)
				{
					bones[VertexID].BoneIDs[index] = BoneIndex;
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

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//Get Material & Textures
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	textures = LoadMaterialTextures(material,
		aiTextureType::aiTextureType_DIFFUSE,
		scene);

	m_Meshes.emplace_back(vertices, indices, textures, transformMatrix);
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

	int animationNum = scene->mNumAnimations;
	for (int i = 0; i < animationNum; i++) {
		aiAnimation *anim = scene->mAnimations[i];
		ProcessAnimation(name, anim, scene);
	}
}

void AnimationImporter::ProcessAnimation(const std::string & name, aiAnimation * anim, const aiScene * scene)
{
	auto& boneIdMap = m_BaseModel->GetBoneIdMap();
	auto& boneOffsets = m_BaseModel->GetBoneOffsets();
	auto clip = Pool::CreateInstance<AnimationClip>();
	clip->Name = name;
	clip->mChannel.resize(boneIdMap.size());
	clip->mTickPerSecond = (float)(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);
	clip->mDuration = (float)anim->mDuration;

	int numChannel = anim->mNumChannels;
	for (int i = 0; i < numChannel; i++) {
		aiNodeAnim *ainodeAnim = anim->mChannels[i];
		BoneChannel channel;

		channel.mChannelName = ainodeAnim->mNodeName.data;
		size_t pos = channel.mChannelName.find("_$Assimp");	//불필요한 문장 제거
		if (pos != std::string::npos) 
			channel.mChannelName = channel.mChannelName.substr(0, pos);
		auto it = boneIdMap.find(channel.mChannelName);
		if (it != boneIdMap.end()) {
			channel.mBoneIndex = it->second;
		}

		channel.mNumPositionKeys = ainodeAnim->mNumPositionKeys;
		channel.mNumRotationKeys = ainodeAnim->mNumRotationKeys;
		channel.mNumScaleKeys = ainodeAnim->mNumScalingKeys;
		channel.mBoneOffset = boneOffsets[channel.mBoneIndex];

		for (int j = 0; j < channel.mNumPositionKeys; j++) {
			auto& posKey = ainodeAnim->mPositionKeys[j];
			auto& posValue = posKey.mValue;

			channel.mPositionKeys.emplace_back(posValue.x, posValue.y, posValue.z, posKey.mTime);
		}

		for (int j = 0; j < channel.mNumRotationKeys; j++) {
			auto& rotKey = ainodeAnim->mRotationKeys[j];
			auto& rotValue = rotKey.mValue;

			channel.mRotationKeys.emplace_back(rotValue.x, rotValue.y, rotValue.z, rotValue.w, rotKey.mTime);
		}

		for (int j = 0; j < channel.mNumScaleKeys; j++) {
			auto& scaleKey = ainodeAnim->mScalingKeys[j];
			auto& scaleValue = scaleKey.mValue;

			channel.mScaleKeys.emplace_back(scaleValue.x, scaleValue.y, scaleValue.z, scaleKey.mTime);
		}

		DirectX::XMMATRIX globalInverseTransform = GetAiMatrixData(scene->mRootNode->mTransformation);
		globalInverseTransform = DirectX::XMMatrixTranspose(globalInverseTransform);
		DirectX::XMVECTOR d = DirectX::XMMatrixDeterminant(globalInverseTransform);
		globalInverseTransform = DirectX::XMMatrixInverse(&d, globalInverseTransform);
		channel.mGlobalInverseTransform = globalInverseTransform;
		clip->mChannel[channel.mBoneIndex] = channel;
	}

	aiNode * ainode = scene->mRootNode;

	ProcessBoneHierarchy(ainode, clip.get(), nullptr, DirectX::XMMatrixIdentity());

	clip->mNumChannel = (short)clip->mChannel.size();
	for (auto& channel : clip->mChannel) {
		channel.mNumChildBone = (short)channel.mChildBoneIndex.size();
	}
}

void AnimationImporter::ProcessBoneHierarchy(aiNode * node, AnimationClip * animClip, BoneChannel * parentBone, const DirectX::XMMATRIX & parentTransform)
{
	auto& boneIdMap = m_BaseModel->GetBoneIdMap();
	std::string nodeName = node->mName.data;
	int nodeIndex = -1;
	DirectX::XMMATRIX nodeTransform = GetAiMatrixData(node->mTransformation);
	nodeTransform = DirectX::XMMatrixTranspose(nodeTransform);

	BoneChannel * currentBoneChannel = nullptr;

	auto it = boneIdMap.find(nodeName);
	if (it != boneIdMap.end()) {
		nodeIndex = it->second;
		currentBoneChannel = &animClip->mChannel[nodeIndex];
	}

	int childNum = node->mNumChildren;

	if (currentBoneChannel != nullptr) {
		if (parentBone != nullptr)
			parentBone->mChildBoneIndex.push_back(currentBoneChannel->mBoneIndex);

		currentBoneChannel->mParentNodeTransform = parentTransform;
		DirectX::XMMATRIX globalTransform = DirectX::XMMatrixIdentity();

		for (int i = 0; i < childNum; i++) {
			aiNode * childNode = node->mChildren[i];
			std::string childName = childNode->mName.data;

			if (boneIdMap.find(childName) == boneIdMap.end()) {
				ProcessBoneHierarchy(childNode, animClip, currentBoneChannel, globalTransform);
				continue;
			}

			ProcessBoneHierarchy(childNode, animClip, currentBoneChannel, globalTransform);
		}
	}
	else {
		DirectX::XMMATRIX globalTransform = nodeTransform * parentTransform;

		for (int i = 0; i < childNum; i++) {
			aiNode * childNode = node->mChildren[i];

			ProcessBoneHierarchy(childNode, animClip, parentBone, globalTransform);
		}
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

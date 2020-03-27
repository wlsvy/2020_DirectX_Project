#include "Model.h"

#include "../AnimationClip.h"
#include "../Component/Animator.h"
#include "../Engine/ModuleResource.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

bool Model::Initialize(
	const std::string & filePath, 
	std::vector<AnimationClip>* _animClipDestination, 
	std::map<std::string, int>& _textureMap, 
	std::vector<Texture>& _textureVec)
{
	try
	{
		if (!this->LoadModel(filePath, _animClipDestination, _textureMap, _textureVec))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool Model::Initialize(const std::string & filePath,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	try
	{
		if (!this->LoadModel(filePath, _textureMap, _textureVec))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool Model::Initialize(std::vector<Vertex3D> * VertexBuffer,
	std::vector<DWORD> * IndexBuffer,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	meshes.push_back(ProcessMesh(VertexBuffer, IndexBuffer, _textureMap, _textureVec));
	return true;
}

bool Model::Initialize(Vertex3D * _VertexBuffer,
	const UINT _vertexSize,
	DWORD * _IndexBuffer,
	const UINT _indexSize,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	meshes.push_back(ProcessMesh(_VertexBuffer, _vertexSize, _IndexBuffer, _indexSize, _textureMap, _textureVec));
	return true;
}

bool Model::Initialize(Vertex3D * _VertexBuffer,
	const UINT _vertexSize,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	meshes.push_back(ProcessMesh(_VertexBuffer, _textureMap, _textureVec));

	return true;
}

void Model::Draw(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix)
{
	Module::GetDeviceContext().VSSetConstantBuffers(0, 1, Module::GetVertexCB().GetAddressOf());
	Module::GetDeviceContext().GSSetConstantBuffers(0, 1, Module::GetVertexCB().GetAddressOf());

	for (int i = 0; i < meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		Module::GetVertexCB().data.wvpMatrix = meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
		Module::GetVertexCB().data.worldMatrix = meshes[i].GetTransformMatrix() * worldMatrix; //Calculate World Matrix
		Module::GetVertexCB().data.vpMatrix = viewProjectionMatrix;
		Module::GetVertexCB().ApplyChanges();
		meshes[i].Draw();
	}
}

void Model::Draw_WireFrame(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix)
{
	Module::GetDeviceContext().VSSetConstantBuffers(0, 1, Module::GetVertexCB().GetAddressOf());
	Module::GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	
	for (int i = 0; i < meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		Module::GetVertexCB().data.wvpMatrix = meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
		Module::GetVertexCB().data.worldMatrix = meshes[i].GetTransformMatrix() * worldMatrix; //Calculate World Matrix
		Module::GetVertexCB().data.vpMatrix = viewProjectionMatrix;
		Module::GetVertexCB().ApplyChanges();
		meshes[i].Draw();
	}
}

void Model::Draw_skinnedMesh(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix, Animator * _animator)
{
	Module::GetDeviceContext().VSSetConstantBuffers(0, 1, Module::GetVertexCB().GetAddressOf());
	
	if (_animator != nullptr) {
		CopyMemory(Module::GetModelBoneCB().data.boneTransform, _animator->mAnimResult.data(), _animator->mAnimResult.size() * sizeof(DirectX::XMMATRIX));
		Module::GetModelBoneCB().ApplyChanges();

		Module::GetDeviceContext().VSSetConstantBuffers(1, 1, Module::GetModelBoneCB().GetAddressOf());
	}

	for (int i = 0; i < meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		Module::GetVertexCB().data.wvpMatrix = meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
		Module::GetVertexCB().data.worldMatrix = meshes[i].GetTransformMatrix() * worldMatrix; //Calculate World Matrix
		Module::GetVertexCB().ApplyChanges();
		meshes[i].Draw();
	}
}

void Model::Draw_BillBoard(const DirectX::XMMATRIX & _worldMatrix, const DirectX::XMMATRIX & _viewProjectionMatrix, float _height, float _width)
{
	Module::GetDeviceContext().VSSetConstantBuffers(0, 1, Module::GetVertexCB().GetAddressOf());

	for (int i = 0; i < meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		Module::GetVertexCB().data.wvpMatrix = meshes[i].GetTransformMatrix() * _worldMatrix * _viewProjectionMatrix; //Calculate World-View-Projection Matrix
		Module::GetVertexCB().data.worldMatrix = meshes[i].GetTransformMatrix() * _worldMatrix; //Calculate World Matrix
		Module::GetVertexCB().ApplyChanges();
		meshes[i].Draw();
	}
}

bool Model::LoadModel(const std::string & filePath,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	this->directory = StringHelper::GetDirectoryFromPath(filePath);

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene, DirectX::XMMatrixIdentity(), _textureMap, _textureVec);

	return true;
}

bool Model::LoadModel(const std::string & filePath,
	std::vector<AnimationClip> * _animClipDestination,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	this->directory = StringHelper::GetDirectoryFromPath(filePath);
	
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);


	if (pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene, DirectX::XMMatrixIdentity(), _textureMap, _textureVec);

	if (!pScene->HasAnimations()) return true; //애니메이션 없을 때 함수 종료


	//추가 애니메이션 처리
	int animationNum = pScene->mNumAnimations;

	
	for (int i = 0; i < animationNum; i++) {
		aiAnimation *pAnimation = pScene->mAnimations[i];
		ProcessAnimation(pAnimation, pScene, _animClipDestination);
	}
		
	return true;
}

void Model::ProcessNode(aiNode * node,
	const aiScene * scene,
	const DirectX::XMMATRIX & parentTransformMatrix,
	std::map<std::string, int> & _textureMap,
	std::vector<Texture> & _textureVec)
{
	DirectX::XMMATRIX nodeTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformMatrix, _textureMap, _textureVec));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTransformMatrix, _textureMap, _textureVec);
	}
}

Mesh Model::ProcessMesh(
	aiMesh * mesh, 
	const aiScene * scene, 
	const DirectX::XMMATRIX & transformMatrix,
	std::map<std::string, int>& _textureMap,
	std::vector<Texture>& _textureVec)
{
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;
	std::vector<Texture*> textures;
	std::vector<MyCustom::Vertex_Bone_Data> VB_buffer;
	std::vector<Vertex3D_BoneWeight> vertices_skinned;

	VB_buffer.resize(mesh->mNumVertices);

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

	//Get Material & Textures
	//_textureVec
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	textures = LoadMaterialTextures(material,
									aiTextureType::aiTextureType_DIFFUSE,
									scene, 
									_textureMap,
									_textureVec);

	if(!mesh->HasBones())
	return Mesh(vertices, indices, textures, transformMatrix, MyCustom::DEFAULT);

	//Get Bones
	UINT BoneIndex = 0;

	for (UINT i = 0; i < mesh->mNumBones; i++) {
		aiBone *bone = mesh->mBones[i];
		std::string boneName = bone->mName.data;
		
		if (m_Bone_Name_Map.find(boneName) == m_Bone_Name_Map.end()) {
			BoneIndex = mBoneCount;
			mBoneCount++;
			MyCustom::Bone boneInfo;
			mBoneBuffer.push_back(boneInfo);
			mBoneBuffer[BoneIndex].BoneOffset = MyCustom::GetAiMatrixData(bone->mOffsetMatrix); //수식 조심
			mBoneBuffer[BoneIndex].BoneOffset = DirectX::XMMatrixTranspose(mBoneBuffer[BoneIndex].BoneOffset);
			m_Bone_Name_Map[boneName] = BoneIndex;
		}
		else {
			BoneIndex = m_Bone_Name_Map[boneName];
		}

		for (UINT j = 0; j < bone->mNumWeights; j++) {
			UINT VertexID = bone->mWeights[j].mVertexId;
			float Weight = bone->mWeights[j].mWeight;
			VB_buffer[VertexID].AddBoneData(BoneIndex, Weight);
			//vertices_skinned[VertexID]
		}
	}

	for (UINT i = 0; i < vertices.size(); i++)
	{
		Vertex3D_BoneWeight vertex(vertices[i].pos, vertices[i].texCoord, vertices[i].normal);

		vertex.boneIDs = DirectX::XMINT4(
			VB_buffer[i].BoneIDs[0], VB_buffer[i].BoneIDs[1], 
			VB_buffer[i].BoneIDs[2], VB_buffer[i].BoneIDs[3]);
		vertex.boneWeights = DirectX::XMFLOAT4(
			VB_buffer[i].BoneWeights[0], VB_buffer[i].BoneWeights[1], 
			VB_buffer[i].BoneWeights[2], VB_buffer[i].BoneWeights[3]);
		

		vertices_skinned.push_back(vertex);
	}

	return Mesh(vertices_skinned, indices, textures, transformMatrix);
}

Mesh Model::ProcessMesh(std::vector<Vertex3D>* _vertexBuffer, std::vector<DWORD>* _indexBuffer, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec)
{
	std::vector<Texture*> textures;
	int TextureID = _textureMap["White Texture"];
	textures.push_back(&_textureVec[TextureID]);

	return Mesh(*_vertexBuffer, *_indexBuffer, textures, DirectX::XMMatrixIdentity(), MyCustom::WITHOUT_TEXTURE);
}

Mesh Model::ProcessMesh(std::vector<Vertex3D_BoneWeight>* _vertexBuffer, std::vector<DWORD>* _indexBuffer, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec)
{
	std::vector<Texture*> textures;
	int TextureID = _textureMap["White Texture"];
	textures.push_back(&_textureVec[TextureID]);

	return Mesh(*_vertexBuffer, *_indexBuffer, textures, DirectX::XMMatrixIdentity());
}

Mesh Model::ProcessMesh(Vertex3D * _vertexBuffer, const int _vertexSize, DWORD * _indexBuffer, const int _indexSize, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec)
{
	std::vector<Texture*> textures;
	int TextureID = _textureMap["White Texture"];
	textures.push_back(&_textureVec[TextureID]);

	return Mesh(_vertexBuffer, _vertexSize, _indexBuffer, _indexSize, textures, DirectX::XMMatrixIdentity(), MyCustom::WITHOUT_TEXTURE);
}

Mesh Model::ProcessMesh(Vertex3D * _vertex, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec)
{
	std::vector<Texture*> textures;
	int TextureID = _textureMap["White Texture"];
	textures.push_back(&_textureVec[TextureID]);

	return Mesh(*_vertex, textures, DirectX::XMMatrixIdentity());
}

void Model::ProcessAnimation(aiAnimation * _aiAnim, const aiScene * _aiScene, std::vector<AnimationClip> * _animClipDestination)
{
	AnimationClip animationClip;
	animationClip.mClipName = _aiAnim->mName.data;
	animationClip.mChannel.resize(m_Bone_Name_Map.size());
	animationClip.mTickPerSecond = (float)(_aiAnim->mTicksPerSecond != 0 ? _aiAnim->mTicksPerSecond : 25.0f);
	animationClip.mDuration = (float)_aiAnim->mDuration;

	int numChannel = _aiAnim->mNumChannels;
	for (int i = 0; i < numChannel; i++) {
		aiNodeAnim *ainodeAnim = _aiAnim->mChannels[i];
		BoneChannel channel;
		
		channel.mChannelName = ainodeAnim->mNodeName.data;
		if (m_Bone_Name_Map.find(channel.mChannelName) != m_Bone_Name_Map.end()) {
			channel.mBoneIndex = m_Bone_Name_Map[channel.mChannelName];
		}

		channel.mNumPositionKeys = ainodeAnim->mNumPositionKeys;
		channel.mNumRotationKeys = ainodeAnim->mNumRotationKeys;
		channel.mNumScaleKeys = ainodeAnim->mNumScalingKeys;
		channel.mBoneOffset = mBoneBuffer[channel.mBoneIndex].BoneOffset;

		for (int j = 0; j < channel.mNumPositionKeys; j++) {
			aiVectorKey aiPosKey = ainodeAnim->mPositionKeys[j];
			PositionKey positionkey;

			positionkey.mTime = (float)aiPosKey.mTime;
			positionkey.mPosition = DirectX::XMFLOAT3(aiPosKey.mValue.x, aiPosKey.mValue.y, aiPosKey.mValue.z);

			channel.mPositionKeys.push_back(positionkey);
		}

		for (int j = 0; j < channel.mNumRotationKeys; j++) {
			aiQuatKey aiRotkey = ainodeAnim->mRotationKeys[j];
			RotationKey rotationkey;

			rotationkey.mTime = (float)aiRotkey.mTime;
			rotationkey.mQuaternion = DirectX::XMFLOAT4(aiRotkey.mValue.x, aiRotkey.mValue.y, aiRotkey.mValue.z, aiRotkey.mValue.w);

			channel.mRotationKeys.push_back(rotationkey);
		}

		for (int j = 0; j < channel.mNumScaleKeys; j++) {
			aiVectorKey aiScaleKey = ainodeAnim->mScalingKeys[j];
			ScaleKey scalekey;

			scalekey.mTime = (float)aiScaleKey.mTime;
			scalekey.mScale = DirectX::XMFLOAT3(aiScaleKey.mValue.x, aiScaleKey.mValue.y, aiScaleKey.mValue.z);

			channel.mScaleKeys.push_back(scalekey);
		}
		
		DirectX::XMMATRIX globalInverseTransform = MyCustom::GetAiMatrixData(_aiScene->mRootNode->mTransformation);
		globalInverseTransform = DirectX::XMMatrixTranspose(globalInverseTransform);
		DirectX::XMVECTOR d = DirectX::XMMatrixDeterminant(globalInverseTransform);
		globalInverseTransform = DirectX::XMMatrixInverse(&d, globalInverseTransform);
		channel.mGlobalInverseTransform = globalInverseTransform;
		animationClip.mChannel[channel.mBoneIndex] = channel;
	}
	
	aiNode * ainode = _aiScene->mRootNode;

#pragma region test
	std::vector<std::string> ddd;
	nodeName(&ddd, ainode);
#pragma endregion

	ProcessBoneHierarchy(ainode, &animationClip, nullptr, DirectX::XMMatrixIdentity());

	animationClip.mNumChannel = (short)animationClip.mChannel.size();
	for (int i = 0; i < animationClip.mChannel.size(); i++) {
		//자식 개수 미리 계산해두기
		animationClip.mChannel[i].mNumChildBone = (short)animationClip.mChannel[i].mChildBoneIndex.size();
	}

	_animClipDestination->push_back(animationClip);
}

void Model::ProcessBoneHierarchy(aiNode * _aiNode, AnimationClip * _animClip, BoneChannel * _parentBone, const DirectX::XMMATRIX & _parentTransform)
{
	std::string nodeName = _aiNode->mName.data;
	int nodeIndex = -1;
	DirectX::XMMATRIX nodeTransform = MyCustom::GetAiMatrixData(_aiNode->mTransformation);
	nodeTransform = DirectX::XMMatrixTranspose(nodeTransform);

#pragma region test
	//DirectX::XMFLOAT4X4 tmat; DirectX::XMStoreFloat4x4(&tmat, nodeTransform);
#pragma endregion

	BoneChannel * currentBoneChannel = nullptr;

	if (m_Bone_Name_Map.find(nodeName) != m_Bone_Name_Map.end()) {
		nodeIndex = m_Bone_Name_Map[nodeName];
		currentBoneChannel = &_animClip->mChannel[nodeIndex];
	}

	int childNum = _aiNode->mNumChildren;

	if (currentBoneChannel != nullptr) {
		if(_parentBone != nullptr)
		_parentBone->mChildBoneIndex.push_back(currentBoneChannel->mBoneIndex);

		currentBoneChannel->mParentNodeTransform = _parentTransform;
		DirectX::XMMATRIX globalTransform = DirectX::XMMatrixIdentity();

		for (int i = 0; i < childNum; i++) {
			aiNode * childNode = _aiNode->mChildren[i];
			std::string childName = childNode->mName.data;

			if (m_Bone_Name_Map.find(childName) == m_Bone_Name_Map.end()) {
				ProcessBoneHierarchy(childNode, _animClip, currentBoneChannel, globalTransform);
				continue;
			}

			/*int childIndex = m_Bone_Name_Map[childName];
			BoneChannel * childPtr = &_animClip->mChannel[childIndex];
			currentBoneChannel->mChildBoneIndex.push_back(childPtr);*/

			ProcessBoneHierarchy(childNode, _animClip, currentBoneChannel, globalTransform);
		}
		
	}
	else {
		DirectX::XMMATRIX globalTransform = nodeTransform * _parentTransform;

		for (int i = 0; i < childNum; i++) {
			aiNode * childNode = _aiNode->mChildren[i];
			

			ProcessBoneHierarchy(childNode, _animClip, _parentBone, globalTransform);
		}
	}



}

Mesh Model::ProcessTerrain(std::vector<Vertex3D> * terrainVertexBuffer, std::vector<DWORD> * terrainIndexBuffer)
{
	std::vector<Texture*> textures;

	return Mesh(*terrainVertexBuffer, *terrainIndexBuffer, textures, DirectX::XMMatrixIdentity(), MyCustom::WITHOUT_TEXTURE);
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
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

std::vector<Texture*> Model::LoadMaterialTextures(
	aiMaterial * pMaterial, 
	aiTextureType textureType, 
	const aiScene * pScene,
	std::map<std::string, int>& _textureMap,
	std::vector<Texture> & _TextureVec)
{
	std::vector<Texture*> texturePtrVec;
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
				int TextureID = _textureMap["Black Texture"];
				texturePtrVec.push_back(&_TextureVec[TextureID]);
				return texturePtrVec;
			}

			std::string textureName = "Material Color" + std::to_string(_TextureVec.size());
			_TextureVec.emplace_back(&Module::GetDevice(), Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType);
			_TextureVec.back().Name = textureName;
			_TextureVec.back().ID = _TextureVec.size() - 1;
			_textureMap.insert(std::make_pair(textureName, _TextureVec.back().ID));
			texturePtrVec.push_back(&_TextureVec.back());
			return texturePtrVec;
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
				Texture embeddedIndexedTexture(&Module::GetDevice(),
												reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
												pScene->mTextures[index]->mWidth,
												textureType);
				std::string textureName = std::string(pScene->mTextures[index]->mFilename.C_Str());
				_TextureVec.push_back(std::move(embeddedIndexedTexture));
				_TextureVec.back().Name = textureName;
				_TextureVec.back().ID = _TextureVec.size() - 1;
				_textureMap.insert(std::make_pair(textureName, _TextureVec.back().ID));
				texturePtrVec.push_back(&_TextureVec.back());
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(&Module::GetDevice(),
										reinterpret_cast<uint8_t*>(pTexture->pcData),
										pTexture->mWidth,
										textureType);
				std::string textureName = std::string(pTexture->mFilename.C_Str());
				_TextureVec.push_back(std::move(embeddedTexture));
				_TextureVec.back().Name = textureName;
				_TextureVec.back().ID = _TextureVec.size() - 1;
				_textureMap.insert(std::make_pair(textureName, _TextureVec.back().ID));
				texturePtrVec.push_back(&_TextureVec.back());
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(&Module::GetDevice(), filename, textureType);
				std::string textureName = std::string(path.C_Str());
				_TextureVec.push_back(std::move(diskTexture));
				_TextureVec.back().Name = textureName;
				_TextureVec.back().ID = _TextureVec.size() - 1;
				_textureMap.insert(std::make_pair(textureName, _TextureVec.back().ID));
				texturePtrVec.push_back(&_TextureVec.back());
				break;
			}
			}
		}
	}

	if (texturePtrVec.size() == 0)
	{
		int TextureID = _textureMap["Unhandled Texture"];
		texturePtrVec.push_back(&_TextureVec[TextureID]);
	}
	return texturePtrVec;

}

int Model::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

void Model::nodeName(std::vector<std::string>* na, aiNode * node)
{
	na->push_back(node->mName.data);

	for (int i = 0; i < node->mNumChildren; i++) {
		nodeName(na, node->mChildren[i]);
	}
}

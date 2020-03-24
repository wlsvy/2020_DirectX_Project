#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
<<<<<<< HEAD:DX_study/DX_study/Engine/SceneManager.h
=======
#include "Util/Singleton.h"
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/SceneManager.h

class Engine; //include 정의 조심
class PhysicsModule;
class GraphicsManager;
struct GAMEOBJECT_INIT_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class GameObject_v2;
struct TimeInfo;
class Component;
class Model;
class PixelShader;
class VertexShader;
class ScriptBehaviour;
class Light_ver2;
class Terrain;
class Collider;
class Animator;
class Transform;
class VertexShader;
class PixelShader;
class GeometryShader;

class ShaderBuffer {
public:
	std::vector<PixelShader> pixelShaderBuffer;
	std::vector<VertexShader> vertexShaderBuffer;
};

class ModelBuffer {
public:
	std::vector<Model*> buffer;
	~ModelBuffer();
};

class TerrainModelBuffer {
public:
	std::vector<Model*> buffer;
	~TerrainModelBuffer();
};

class SceneManager {
	friend class Engine;
public:
	SceneManager();
	~SceneManager();

	bool DestoryGameObject(GameObject_v2& gameObject);
	void OnGui();
	GameObject_v2 * getUIselectedObj();

	Model * getModelByName(const std::string & _str);

	TimeInfo * const Time;

private:
<<<<<<< HEAD:DX_study/DX_study/Engine/SceneManager.h
	
=======
	SceneManager(Engine * const engine_ptr, 
		TimeInfo * const timeInfo, 
		PhysicsManager * const _physcisManager);
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/SceneManager.h
	void Custom_Test_Obj_Set();
	
	GameObject_v2* AddGameObject(GAMEOBJECT_INIT_DESC & desc);
	void Update();
	void RecursiveTransformUpdate(Transform * _transform, DirectX::XMMATRIX & _parentMatrix);

	void UIrecursiveTransformCheck(Transform * _transform);
	

<<<<<<< HEAD:DX_study/DX_study/Engine/SceneManager.h
	std::shared_ptr<GameObject_v2> m_RootGameObject;
	std::vector<std::shared_ptr<GameObject_v2>> m_GameObjects;
=======

	std::vector<std::shared_ptr<GameObject_v2>> gameObjectBuffer;
	Engine * const engine;
	PhysicsManager * const mPhysicsManager;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/SceneManager.h
	ModelBuffer modelBuffer;
	ShaderBuffer shaderBuffer;
	TerrainModelBuffer terrainBuffer;

	Transform* mWorldTransform;
	DirectX::XMMATRIX mWorldMatrix;
	
	int ObjIDcontributor = 1;
	int ComponentIDcontributor = 1;
	int mUI_Selected_Transform_ID = -1;
	Transform* mUI_Selectd_Transform_Ptr = nullptr;
};
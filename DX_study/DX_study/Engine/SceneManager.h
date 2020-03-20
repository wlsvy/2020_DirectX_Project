#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "../Util/Singleton.h"

class Engine; //include 정의 조심
class PhysicsManager;
class GraphicsManager;
struct GAMEOBJECT_INIT_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class GameObject_v2;
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
	//bool Initialize();
	//void LoadObject_Info();

	void ClassifyComponent(Component * _component, GameObject_v2 *_destination); //똑같은 컴포넌트를 여러 번 분류하는 경우 조심.
	void Component_Valid_Test();
	bool Destory_GameObject(GameObject_v2 * _gameObject);
	void OnGui();
	GameObject_v2 * getUIselectedObj();
	PhysicsManager * getPhysicsManagerPtr();
	GraphicsManager * getGraphicsManagerPtr();
	ID3D11Device * getDevicePtr();
	ID3D11DeviceContext * getDeviceContextPtr();

	VertexShader * getVshaderByName(const std::string & _str);
	PixelShader * getPshaderByName(const std::string & _str);
	GeometryShader * getGshaderByName(const std::string & _str);
	Model * getModelByName(const std::string & _str);

private:
	SceneManager(PhysicsManager * const _physcisManager);
	void Custom_Test_Obj_Set();
	~SceneManager();
	GameObject_v2* AddGameObject(GAMEOBJECT_INIT_DESC & desc);
	void Update();
	void RecursiveTransformUpdate(Transform * _transform, DirectX::XMMATRIX & _parentMatrix);

	void UIrecursiveTransformCheck(Transform * _transform);
	


	std::vector<std::shared_ptr<GameObject_v2>> gameObjectBuffer;
	PhysicsManager * const mPhysicsManager;
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
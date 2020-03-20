#pragma once
#include "Object.h"

class GameObject_v2; // include 컴파일 주의할것. 아직 포함 안시킨 상태
class PhysicsManager;
class GraphicsManager;
class Transform;
class Renderer;
struct ID3D11Device;
struct ID3D11DeviceContext;
template<class T>
class ConstantBuffer;
struct CB_VS_boneData;

enum COMPONENT_TYPE {
	COMPONENT_SCRIPT =		0, 
	COMPONENT_TRANSFORM =	1,
	COMPONENT_RENDERER =	2, 
	COMPONENT_COLLISION =	3, 
	COMPONENT_ANIMATION =	4, 
	COMPONENT_LIGHT =		5,
	COMPONENT_TERRAIN =		6};

struct COMPONENT_INIT_DESC {
	COMPONENT_INIT_DESC() {}
	COMPONENT_INIT_DESC(GameObject_v2 * _gameObj, Transform * _Transform, Renderer * _Renderer) {
		mGameObj = _gameObj;
		mTransform = _Transform;
		mRenderer = _Renderer;
	}

	GameObject_v2 * mGameObj = nullptr;
	Transform * mTransform = nullptr;
	Renderer * mRenderer = nullptr;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mDeviceContext = nullptr;
	ConstantBuffer<CB_VS_boneData> * mBoneData = nullptr;
	PhysicsManager * mPhysicsManager = nullptr;
	GraphicsManager * mGraphicsManager = nullptr;
	
	//ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;
};

class Component abstract : public Object{
	friend class Engine;
	friend class SceneManager;
public:
	Component(const COMPONENT_INIT_DESC & desc);

	int getComponentID();
	COMPONENT_TYPE getComponentType();

	virtual void OnGui() {}
	virtual void OnDebugDraw() {}

	
	char mComponentName[40];
	
	bool enabled = true;
	GameObject_v2 * const gameObject;
protected:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

	int mComponentID = -1;
	COMPONENT_TYPE mComponentType;
};

#include "BaseComponentInclude.h"

#pragma once
#include "Object.h"
#include "ComponentTypeTraits.h"

class GameObject_v2; // include 컴파일 주의할것. 아직 포함 안시킨 상태
class PhysicsModule;
class GraphicsManager;
struct TimeInfo;
class Transform;
class Renderer;
struct ID3D11Device;
struct ID3D11DeviceContext;
template<class T>
class ConstantBuffer;
struct CB_VS_boneData;

<<<<<<< HEAD
=======
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
	COMPONENT_INIT_DESC(GameObject_v2 * _gameObj, TimeInfo * _Time, Transform * _Transform, Renderer * _Renderer) {
		mTime = _Time;
		mGameObj = _gameObj;
		mTransform = _Transform;
		mRenderer = _Renderer;
	}

	GameObject_v2 * mGameObj = nullptr;
	TimeInfo * mTime = nullptr;
	Transform * mTransform = nullptr;
	Renderer * mRenderer = nullptr;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mDeviceContext = nullptr;
	ConstantBuffer<CB_VS_boneData> * mBoneData = nullptr;
	PhysicsManager * mPhysicsManager = nullptr;
	GraphicsManager * mGraphicsManager = nullptr;
	
	//ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;
};

>>>>>>> parent of cb3481a... refactoring
class Component abstract : public Object{
	friend class Engine;
	friend class SceneManager;
public:
	Component(GameObject_v2 & obj);

	int getComponentID();

	virtual void OnGui() {}
	virtual void OnDebugDraw() {}

	
	char Name[40];
	
<<<<<<< HEAD
	bool Enabled = true;
	GameObject_v2& GameObject;
=======
	bool enabled = true;
	GameObject_v2 * const gameObject;
	TimeInfo * const Time;
>>>>>>> parent of cb3481a... refactoring
protected:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

<<<<<<< HEAD
	int m_ID = -1;
=======
	int mComponentID = -1;
	COMPONENT_TYPE mComponentType;

	
>>>>>>> parent of cb3481a... refactoring
};

#include "BaseComponentInclude.h"

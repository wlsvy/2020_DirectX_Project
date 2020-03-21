#pragma once
#include "Object.h"
#include "ComponentTypeTraits.h"

class GameObject_v2; // include 컴파일 주의할것. 아직 포함 안시킨 상태
class PhysicsModule;
class GraphicsManager;
class Transform;
class Renderer;
struct ID3D11Device;
struct ID3D11DeviceContext;
template<class T>
class ConstantBuffer;
struct CB_VS_boneData;

class Component abstract : public Object{
	friend class Engine;
	friend class SceneManager;
public:
	Component(GameObject_v2 & obj);

	int getComponentID();

	virtual void OnGui() {}
	virtual void OnDebugDraw() {}

	
	char Name[40];
	
	bool enabled = true;
	GameObject_v2& gameObject;
protected:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

	int m_ID = -1;
};

#include "BaseComponentInclude.h"

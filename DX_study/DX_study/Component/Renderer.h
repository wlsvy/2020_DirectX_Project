#pragma once
#include <string>

#include "Component.h"

class Model;
class VertexShader;
class PixelShader;
class GeometryShader;
class Animator;
namespace DirectX {
	struct XMMATRIX;
}

class Renderer : public Component {
	friend class GraphicsManager;
public:
	Renderer(GameObject_v2 & obj);
	bool Initialize(
		Model * model_ptr,
		VertexShader* _vshader,
		PixelShader* _pshader,
		GeometryShader* _gshader);
	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix) const;
	void SetModel(Model * _model);

	void OnGui() override;

	//Properties
	bool enabled = true;
	int modelID = -1;
	std::string model_filepath;

	bool drawWireFrame = false;
	bool drawSkinnedMesh = false;
	Animator * animator = nullptr;

	Transform& transform;

private:
	Model*	m_Model = nullptr;
	VertexShader*	m_Vshader = nullptr;
	PixelShader*	m_Pshader = nullptr;
	GeometryShader* m_Gshader = nullptr;
	

#pragma region Variable - GUI
	bool mShowModelWindow = false;
	bool mShowVshaderWindow = false;
	bool mShowPshaderWindow = false;
	bool mShowGshaderWindow = false;
#pragma endregion
};
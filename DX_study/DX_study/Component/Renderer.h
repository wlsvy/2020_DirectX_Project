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
	Renderer(const COMPONENT_INIT_DESC & desc);
	bool Initialize(
		Model * model_ptr,
		VertexShader* _vshader,
		PixelShader* _pshader,
		GeometryShader* _gshader);
	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix);
	void SetModel(Model * _model);

	void OnGui() override;

	//Properties
	bool enabled = true;
	int modelID = -1;
	std::string model_filepath;

	bool drawWireFrame = false;
	bool drawSkinnedMesh = false;
	Animator * animator = nullptr;
	const COMPONENT_TYPE mType = COMPONENT_RENDERER;
private:
	Model*	mModelPtr = nullptr;
	VertexShader*	mVshaderPtr = nullptr;
	PixelShader*	mPshaderPtr = nullptr;
	GeometryShader* mGshaderPtr = nullptr;
	Transform * const transform;

#pragma region Variable - GUI
	bool mShowModelWindow = false;
	bool mShowVshaderWindow = false;
	bool mShowPshaderWindow = false;
	bool mShowGshaderWindow = false;
#pragma endregion
};
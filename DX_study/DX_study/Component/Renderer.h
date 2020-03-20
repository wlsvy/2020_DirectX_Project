#ifndef RENDERER_H
#define RENDERER_H
#include "Component.h"
#include <string>
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"

class GraphicsManager;

class Renderer : public Component {
	friend class GraphicsManager;
public:
	Renderer(const COMPONENT_INIT_DESC & desc);
	bool Initialize(Model * model_ptr,
		VertexShader* _vshader,
		PixelShader* _pshader,
		GeometryShader* _gshader,
		ID3D11Device* _device,
		ID3D11DeviceContext* _deviceContext);
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
	ID3D11Device * mDevice = nullptr;
	ID3D11DeviceContext * mDeviceContext = nullptr;

	Model*	mModelPtr = nullptr;
	VertexShader*	mVshaderPtr = nullptr;
	PixelShader*	mPshaderPtr = nullptr;
	GeometryShader* mGshaderPtr = nullptr;
	Transform * const transform;
	GraphicsManager * mGraphicsManager;

#pragma region Variable - GUI
	bool mShowModelWindow = false;
	bool mShowVshaderWindow = false;
	bool mShowPshaderWindow = false;
	bool mShowGshaderWindow = false;
#pragma endregion
};

#endif
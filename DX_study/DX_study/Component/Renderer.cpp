#include "Renderer.h"
#include "Animator.h"

void Renderer::SetModel(Model * _model)
{
	mModelPtr = _model;
}

void Renderer::OnGui()
{
	std::string vshader;
	std::string pshader;
	std::string gshader;
	std::string modelstr;
	ImVec2 ButtonSize(100, 25);
	ImGui::Text("Mesh Model : ");
	ImGui::SameLine();
	if (ImGui::Button(modelstr.c_str(), ButtonSize)) {
		mShowModelWindow = !mShowModelWindow;
	}

	ImGui::Text("Vertex Shader : ");
	ImGui::SameLine();
	if (ImGui::Button(mVshaderPtr->shaderName.c_str(), ButtonSize)) {
		mShowVshaderWindow = !mShowVshaderWindow;
	}

	ImGui::Text("Pixel Shader : ");
	ImGui::SameLine();
	if (ImGui::Button(mPshaderPtr->shaderName.c_str(), ButtonSize)) {
		mShowPshaderWindow = !mShowPshaderWindow;
	}

	ImGui::Text("Geometry Shader : ");
	ImGui::SameLine();
	if (mGshaderPtr == nullptr) {
		if (ImGui::Button("Empty", ButtonSize)) {
			mShowGshaderWindow = !mShowGshaderWindow;
		}
	}
	else {
		if (ImGui::Button(mGshaderPtr->shaderName.c_str(), ButtonSize)) {
			mShowGshaderWindow = !mShowGshaderWindow;
		}
	}
	


	
	//마테리얼(색상, 발광, 노말맵, 텍스쳐 등), 그림자 받기, 그림자 생성, 
}

Renderer::Renderer(const COMPONENT_INIT_DESC & desc) :
	Component(desc),
	transform(desc.mTransform)
{ 
	std::strcpy(mComponentName, "Renderer"); 
}

bool Renderer::Initialize(Model * model_ptr,
	VertexShader* _vshader,
	PixelShader* _pshader,
	GeometryShader* _gshader,
	ID3D11Device* _device,
	ID3D11DeviceContext* _deviceContext)
{
	mModelPtr = model_ptr;
	mVshaderPtr = _vshader;
	mPshaderPtr = _pshader;
	mGshaderPtr = _gshader;
	mDevice = _device;
	mDeviceContext = _deviceContext;

	assert("Renderer Component has Null ptr Device or Device Context!!" && mDevice != nullptr && mDeviceContext != nullptr);
	return true;
}

void Renderer::Draw(const DirectX::XMMATRIX & viewProjectionMatrix)
{
	//예외 처리 필요
	mDeviceContext->IASetInputLayout(mVshaderPtr->GetInputLayout()); //IA에 입력할 배치 적용
	mDeviceContext->VSSetShader(mVshaderPtr->GetShader(), NULL, 0); //그릴 때 쓸 셰이더 적용
	mDeviceContext->PSSetShader(mPshaderPtr->GetShader(), NULL, 0); //그릴 때 쓸 셰이더 적용

	if (mGshaderPtr == nullptr) {
		mDeviceContext->GSSetShader(NULL, NULL, 0);
	}
	else {
		mDeviceContext->GSSetShader(mGshaderPtr->GetShader(), NULL, 0);
	}

	if (!drawSkinnedMesh) {
		mModelPtr->Draw(this->transform->worldMatrix, viewProjectionMatrix);
	}
	else {
		mModelPtr->Draw_skinnedMesh(this->transform->worldMatrix, viewProjectionMatrix, animator);
	}
}

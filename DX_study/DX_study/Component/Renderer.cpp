#include "Renderer.h"
#include "Animator.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../Engine/ModuleResource.h"

void Renderer::SetModel(Model * _model)
{
	m_Model = _model;
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
	if (ImGui::Button(m_Vshader->Name.c_str(), ButtonSize)) {
		mShowVshaderWindow = !mShowVshaderWindow;
	}

	ImGui::Text("Pixel Shader : ");
	ImGui::SameLine();
	if (ImGui::Button(m_Pshader->Name.c_str(), ButtonSize)) {
		mShowPshaderWindow = !mShowPshaderWindow;
	}

	ImGui::Text("Geometry Shader : ");
	ImGui::SameLine();
	if (m_Gshader == nullptr) {
		if (ImGui::Button("Empty", ButtonSize)) {
			mShowGshaderWindow = !mShowGshaderWindow;
		}
	}
	else {
		if (ImGui::Button(m_Gshader->Name.c_str(), ButtonSize)) {
			mShowGshaderWindow = !mShowGshaderWindow;
		}
	}
	


	
	//마테리얼(색상, 발광, 노말맵, 텍스쳐 등), 그림자 받기, 그림자 생성, 
}

Renderer::Renderer(GameObject_v2 & obj) :
	Component(obj),
	transform(obj.transform)
{ 
	std::strcpy(Name, "Renderer"); 
}

bool Renderer::Initialize(
	Model * model_ptr,
	VertexShader* _vshader,
	PixelShader* _pshader,
	GeometryShader* _gshader)
{
	m_Model = model_ptr;
	m_Vshader = _vshader;
	m_Pshader = _pshader;
	m_Gshader = _gshader;

	return true;
}

void Renderer::Draw(const DirectX::XMMATRIX & viewProjectionMatrix) const
{
	//예외 처리 필요
	Module::GetDeviceContext().IASetInputLayout(m_Vshader->GetInputLayout()); //IA에 입력할 배치 적용
	Module::GetDeviceContext().VSSetShader(m_Vshader->GetShader(), NULL, 0); //그릴 때 쓸 셰이더 적용
	Module::GetDeviceContext().PSSetShader(m_Pshader->GetShader(), NULL, 0); //그릴 때 쓸 셰이더 적용

	if (m_Gshader == nullptr) {
		Module::GetDeviceContext().GSSetShader(NULL, NULL, 0);
	}
	else {
		Module::GetDeviceContext().GSSetShader(m_Gshader->GetShader(), NULL, 0);
	}

	if (!drawSkinnedMesh) {
		m_Model->Draw(this->transform.worldMatrix, viewProjectionMatrix);
	}
	else {
		m_Model->Draw_skinnedMesh(this->transform.worldMatrix, viewProjectionMatrix, animator);
	}
}

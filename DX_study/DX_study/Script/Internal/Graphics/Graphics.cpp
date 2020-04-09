#include "Graphics.h"

#include <WICTextureLoader.h>
#include <io.h>

#include "imGui/imgui.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_impl_dx11.h"
#include "Model.h"
#include "Shaders.h"
#include "Skybox.h"
#include "BaseGeometry.h"
#include "AnimationClip.h"
#include "../../Util/Time.h"
#include "../Engine/Engine.h"
#include "../Engine/DeviceResources.h"
#include "../Engine/Ui.h"
#include "../Core/ObjectPool.h"
#include "../Core/GameObject.h"
#include "../Core/ImportHelper.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"
#include "../../Component/Custom/CamMove.h"
#include "../../Component/Animator.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"
#include "../../GameObject/Sprite.h"

void TraverseDirectory(const std::string & dirPath, void(Graphics::* callBack)(const std::string &));

bool Graphics::Initialize(HWND hwnd, int width, int height) {
	mainCam = std::make_shared<Camera>();
	light = std::make_shared<Light>();
	gameObject = std::make_shared<GameObject>("First Created");

	this->windowWidth = width;
	this->windowHeight = height;

	if (!m_DeviceResources.Initialize(hwnd, width, height) ||
		!m_DeviceResources.InitializeRenderTarget(width, height)) {
		return false;
	}

	

	TraverseDirectory("hlsl\\VertexShader\\", &Graphics::LoadVertexShader);
	TraverseDirectory("hlsl\\PixelShader\\", &Graphics::LoadPixelShader);
	TraverseDirectory("Data\\Objects\\", &Graphics::LoadModel);
	TraverseDirectory("Data\\Textures\\", &Graphics::LoadTexture);

	BaseGeometry::Initialize();

	m_Skybox = std::make_shared<Skybox>();
	std::string filename[6] = { //순서는 나중에
		"Data\\Skybox\\Test1\\oasisnight_ft_p.png",	// +X
		"Data\\Skybox\\Test1\\oasisnight_bk_p.png",	// -X
		"Data\\Skybox\\Test1\\oasisnight_up_p.png",	// +Y
		"Data\\Skybox\\Test1\\oasisnight_dn_p.png",	// -Y
		"Data\\Skybox\\Test1\\oasisnight_rt_p.png",	// +Z
		"Data\\Skybox\\Test1\\oasisnight_lf_p.png"	// -Z
	};
	if (!m_Skybox->Initialize(filename)) {
		MessageBoxA(NULL, "Initialize Skybox Error", "Error", MB_ICONERROR);
		return false;
	}

	if (!InitializeScene()) {
		MessageBoxA(NULL, "Initialize Scene Error", "Error", MB_ICONERROR);
		return false;
	}
		
	UI::InitImGUI(hwnd);

	return true;
}

void Graphics::RenderFrame()
{
	cb_ps_light.data.dynamicLightColor = light->lightColor;
	cb_ps_light.data.dynamicLightStrength = light->lightStrength;
	cb_ps_light.data.dynamicLightPosition = light->GetTransform().GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light->attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light->attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light->attenuation_c;
	cb_ps_light.ApplyChanges();
	
	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetBaseRenderTargetView(), m_BackgroundColor);
	m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetAuxRenderTargetView(), m_BackgroundColor);
	m_DeviceResources.GetDeviceContext()->ClearDepthStencilView(m_DeviceResources.GetBaseDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_DeviceResources.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);

	m_DeviceResources.GetDeviceContext()->OMSetBlendState(m_DeviceResources.GetBlendState(), m_BlendFactors, 0xFFFFFFFF);
	m_DeviceResources.GetDeviceContext()->PSSetSamplers(0, 1, m_DeviceResources.GetSamplerStateAddr());
}

void Graphics::Draw(const std::shared_ptr<Renderable>& renderer)
{
	if (!renderer->Vshader ||
		!renderer->Pshader)
	{
		return;
	}

	m_DeviceResources.GetDeviceContext()->IASetInputLayout(renderer->Vshader->GetInputLayout());
	m_DeviceResources.GetDeviceContext()->VSSetShader(renderer->Vshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(renderer->Pshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());

	auto& worldMat = renderer->GetGameObject()->GetTransform().GetWorldMatrix();
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	
	if (renderer->Model) {
		DrawModel(renderer->Model, worldMat, wvpMat);
	}
	else if (renderer->SkinnedModel &&
		renderer->Anim &&
		renderer->Anim->Clip) 
	{
		CopyMemory(cb_BoneInfo.data.boneTransform, 
			renderer->Anim->GetAnimResult().data(), 
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		cb_BoneInfo.ApplyChanges();

		m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_BoneInfo.GetAddressOf());
		DrawModel(renderer->SkinnedModel, worldMat, wvpMat);
	}
}

void Graphics::DrawModel(
	const std::shared_ptr<Model>& model, 
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	for (auto& mesh : model->GetMeshes()) {
		DrawMesh(mesh, worldMat, wvpMat);
	}
}
void Graphics::DrawModel(
	const std::shared_ptr<SkinnedModel>& model,
	const DirectX::XMMATRIX & worldMat,
	const DirectX::XMMATRIX & wvpMat)
{
	for (auto& mesh : model->GetMeshes()) {
		cb_vs_vertexshader.data.wvpMatrix = mesh.GetTransformMatrix() * wvpMat; //Calculate World-View-Projection Matrix
		cb_vs_vertexshader.data.worldMatrix = mesh.GetTransformMatrix() * worldMat; //Calculate World Matrix
		cb_vs_vertexshader.ApplyChanges();

		for (auto& texture : mesh.GetTextures()) {
			if (texture.GetType() == aiTextureType::aiTextureType_DIFFUSE) {
				m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());
				break;
			}
		}

		UINT offset = 0;
		m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.GetVertexBuffer().GetAddressOf(), mesh.GetVertexBuffer().StridePtr(), &offset);
		m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh.GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh.GetIndexBuffer().IndexCount(), 0, 0);
	}
}
void Graphics::DrawMesh(
	const Mesh & mesh,
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	cb_vs_vertexshader.data.wvpMatrix = mesh.GetTransformMatrix() * wvpMat; //Calculate World-View-Projection Matrix
	cb_vs_vertexshader.data.worldMatrix = mesh.GetTransformMatrix() * worldMat; //Calculate World Matrix
	cb_vs_vertexshader.ApplyChanges();

	for (auto& texture : mesh.GetTextures()) {
		if (texture.GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());
			break;
		}
	}

	UINT offset = 0;
	m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.GetVertexBuffer().GetAddressOf(), mesh.GetVertexBuffer().StridePtr(), &offset);
	m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh.GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh.GetIndexBuffer().IndexCount(), 0, 0);
}

void Graphics::DrawUI()
{
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	UI::EditorUI(m_DeviceResources.GetAuxRenderTargetSrv());
	
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strenght", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &this->light->lightColor.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &this->light->lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &this->light->attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &this->light->attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &this->light->attenuation_c, 0.01f, 0.0f, 10.0f);
	
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Graphics::DrawSkybox()
{
	auto vs = Pool::Find<VertexShader>("vertexshader_skybox");
	auto ps = Pool::Find<PixelShader>("pixelshader_skybox");
	m_DeviceResources.GetDeviceContext()->IASetInputLayout(vs->GetInputLayout()); //IA에 입력할 배치 적용
	m_DeviceResources.GetDeviceContext()->VSSetShader(vs->GetShader(), NULL, 0); //그릴 때 쓸 셰이더 적용
	m_DeviceResources.GetDeviceContext()->PSSetShader(ps->GetShader(), NULL, 0); //그릴 때 쓸 셰이더 적용
	m_DeviceResources.GetDeviceContext()->GSSetShader(NULL, NULL, 0);

	m_Skybox->Draw(
		DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().GetPositionVector()),
		mainCam->GetViewProjectionMatrix());

	auto model = Pool::Find<Model>("Box");
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().GetPositionVector());
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();

	DrawModel(model, worldMat, wvpMat);
}

void Graphics::SetOmRenderTargetToBase()
{
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(1, m_DeviceResources.GetBaseRenderTargetViewAddress(), m_DeviceResources.GetBaseDepthStencilView());
}

void Graphics::SetOmRenderTargetToAux()
{
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(1, m_DeviceResources.GetAuxRenderTargetViewAddress(), m_DeviceResources.GetBaseDepthStencilView());
}

void Graphics::SwapBuffer()
{
	m_DeviceResources.GetSwapChain()->Present(1, NULL);
}

bool Graphics::InitializeScene()
{
	try {
		//Initialize Constant buffer(s)
		ThrowIfFailed(cb_vs_vertexshader_2d.Initialize(), "Failed to Initialize CB_VS_vertexshader_2d buffer.");
		ThrowIfFailed(cb_vs_vertexshader.Initialize(), "Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(cb_ps_light.Initialize(), "Failed to Initialize cb_ps_light buffer.");
		ThrowIfFailed(cb_BoneInfo.Initialize(), "Failed to Initialize cb_BoneInfo buffer.");

		cb_ps_light.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		cb_ps_light.data.ambientLightStrength = 1.0f;

		gameObject->GetTransform().SetPosition(2.0f, 0.0f, 0.0f);
		gameObject->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
		//gameObject->GetRenderer().Model = Pool::Find<Model>("nanosuit");
		gameObject->GetRenderer().SkinnedModel = Pool::Find<SkinnedModel>("Walking");
		gameObject->GetRenderer().Anim = gameObject->AddComponent<Animator>();
		gameObject->GetRenderer().Anim->Clip = Pool::Find<AnimationClip>("mixamo.com");
		gameObject->GetRenderer().Anim->Play();
		//gameObject->GetRenderer().Vshader = Pool::Find<VertexShader>("vertexshader");
		gameObject->GetRenderer().Vshader = Pool::Find<VertexShader>("skinned_vertex");
		gameObject->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader");
		light->GetRenderer().Model = Pool::Find<Model>("light");
		light->GetRenderer().Vshader = Pool::Find<VertexShader>("vertexshader");
		light->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader_nolight");

		mainCam->GetTransform().SetPosition(0.0f, 0.0f, -2.0f);
		mainCam->SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
		mainCam->AddComponent<CamMove>();
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Graphics::LoadModel(const std::string & filePath)
{
	std::string ext = StringHelper::GetFileExtension(filePath);
	if (ext == "fbx" ||
		ext == "obj")
	{
		if (!Importer::LoadModel(filePath)) {
			MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
			return;
		}
	}
}

void Graphics::LoadVertexShader(const std::string & filePath)
{
	std::string ext = StringHelper::GetFileExtension(filePath);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout3D[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEID", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEWEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Pool::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetBuildPath() + StringHelper::GetNameFromPath(filePath, true) + ".cso",
			layout3D,
			ARRAYSIZE(layout3D)))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Graphics::LoadPixelShader(const std::string & filePath)
{
	std::string ext = StringHelper::GetFileExtension(filePath);
	if (ext == "hlsl")
	{
		auto ps = Pool::CreateInstance<PixelShader>();
		if (!ps->Initialize(Core::GetBuildPath() + StringHelper::GetNameFromPath(filePath, true) + ".cso"))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Graphics::LoadTexture(const std::string & filePath)
{
	std::string ext = StringHelper::GetFileExtension(filePath);
	if (ext == "jpg" ||
		ext == "png" ||
		ext == "bmp")
	{
		Pool::CreateInstance<Texture>(filePath, aiTextureType::aiTextureType_DIFFUSE);
	}
}

void TraverseDirectory(const std::string & dirPath, void(Graphics::* callBack)(const std::string &))
{
	struct _finddata_t fd;
	intptr_t handle;

	std::string filePath = dirPath + "*.*";

	if ((handle = _findfirst(filePath.c_str(), &fd)) != -1L) {
		do {
			if (fd.attrib & _A_SUBDIR &&
				(fd.name != std::string(".")) &&
				(fd.name != std::string("..")))
			{
				std::string subDirPath = dirPath + fd.name + "\\";
				TraverseDirectory(subDirPath, callBack);
				continue;
			}

			(Engine::Get().GetGraphics().*callBack)(dirPath + fd.name);
		} while (_findnext(handle, &fd) == 0);
	}
	_findclose(handle);
}
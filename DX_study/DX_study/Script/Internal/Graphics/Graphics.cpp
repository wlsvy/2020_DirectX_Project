#include "Graphics.h"

#include <WICTextureLoader.h>

#include "imGui/imgui.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_impl_dx11.h"
#include "Model.h"
#include "../../Util/Time.h"
#include "../Engine/DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/GameObject.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"
#include "../../Component/Custom/CamMove.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"
#include "../../GameObject/Sprite.h"

bool Graphics::Initialize(HWND hwnd, int width, int height) {
	mainCam = std::make_shared<Camera>();
	light = std::make_shared<Light>();
	gameObject = std::make_shared<GameObject>("First Created");

	this->windowWidth = width;
	this->windowHeight = height;

	if (!m_DeviceResources.Initialize(hwnd, width, height)) {
		return false;
	}

	if (!InitializeShaders()) {
		MessageBoxA(NULL, "Initialize Shader Error", "Error", MB_ICONERROR);
		return false;
	}

	if (!InitializeScene()) {
		MessageBoxA(NULL, "Initialize Scene Error", "Error", MB_ICONERROR);
		return false;
	}
		

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_DeviceResources.GetDevice(), m_DeviceResources.GetDeviceContext());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{
	static float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	cb_ps_light.data.dynamicLightColor = light->lightColor;
	cb_ps_light.data.dynamicLightStrength = light->lightStrength;
	cb_ps_light.data.dynamicLightPosition = light->GetTransform().GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light->attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light->attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light->attenuation_c;
	cb_ps_light.ApplyChanges();
	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

	m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetBaseRenderTargetView(), bgcolor);
	m_DeviceResources.GetDeviceContext()->ClearDepthStencilView(m_DeviceResources.GetBaseDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_DeviceResources.GetDeviceContext()->IASetInputLayout(this->vertexshader.GetInputLayout());
	m_DeviceResources.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);

	m_DeviceResources.GetDeviceContext()->OMSetBlendState(m_DeviceResources.GetBlendState(), blendFactors, 0xFFFFFFFF);
	m_DeviceResources.GetDeviceContext()->PSSetSamplers(0, 1, m_DeviceResources.GetSamplerStateAddr());
}

void Graphics::Draw(const std::shared_ptr<Renderable>& renderer)
{
	if (!renderer->Vshader ||
		!renderer->Pshader ||
		!renderer->Model)
	{
		return;
	}

	m_DeviceResources.GetDeviceContext()->IASetInputLayout(renderer->Vshader->GetInputLayout());
	m_DeviceResources.GetDeviceContext()->VSSetShader(renderer->Vshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(renderer->Pshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());

	auto& worldMat = renderer->GetGameObject()->GetTransform().GetWorldMatrix();
	renderer->Model->Draw(worldMat, mainCam->GetViewProjectionMatrix());
}

void Graphics::DrawFrameString()
{
	//Draw text + 프레임 측정
	static int fpsCounter = 0;
	static double elapsedTime = 0.0;
	static std::string fpsString = "FPS: 0";

	fpsCounter += 1;
	elapsedTime += Time::GetDeltaTime();
	if (elapsedTime > 1.0f) {
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		elapsedTime = 0.0;
	}
	auto spriteBatch = m_DeviceResources.GetSpriteBatch();
	auto spriteFont = m_DeviceResources.GetSpriteFont();
	spriteBatch->Begin();
	spriteFont->DrawString(
		spriteBatch, 
		StringHelper::StringToWide(fpsString).c_str(), 
		DirectX::XMFLOAT2(0, 0), 
		DirectX::Colors::White, 0.0f, 
		DirectX::XMFLOAT2(0, 0), 
		DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();
}

void Graphics::DrawImGui()
{
	static int counter = 0;
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
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

void Graphics::SwapBuffer()
{
	m_DeviceResources.PresentSwapChain();
}

bool Graphics::InitializeShaders()
{
	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath

#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Debug\\";
#else //x86(win32)
		shaderfolder = L"..\\Debug\\";
#endif 
#else //Release Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Release\\";
#else //x86(win32)
		shaderfolder = L"..\\Release\\";
#endif
#endif

	//3D shader
	D3D11_INPUT_ELEMENT_DESC layout3D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		//{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
	};

	UINT numElements3D = ARRAYSIZE(layout3D);

	if (!vertexshader.Initialize(shaderfolder + L"vertexshader.cso", layout3D, numElements3D))
		return false;

	if (!pixelshader.Initialize(shaderfolder + L"pixelshader.cso"))
		return false;

	//2D shader
	D3D11_INPUT_ELEMENT_DESC layout2D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
	};

	UINT numElements2D = ARRAYSIZE(layout2D);

	if (!vertexshader_2d.Initialize(shaderfolder + L"vertexshader_2d.cso", layout2D, numElements2D))
		return false;

	if (!pixelshader_2d.Initialize(shaderfolder + L"pixelshader_2d.cso"))
		return false;

	if (!pixelshader_nolight.Initialize(shaderfolder + L"pixelshader_nolight.cso"))
		return false;

	if (!pixelshader_2d_discard.Initialize(shaderfolder + L"pixelshader_2d_discard.cso"))
		return false;


	return true;
}



bool Graphics::InitializeScene()
{
	try {
		//Initialize Constant buffer(s)
		ThrowIfFailed(cb_vs_vertexshader_2d.Initialize(), "Failed to Initialize CB_VS_vertexshader_2d buffer.");
		ThrowIfFailed(cb_vs_vertexshader.Initialize(), "Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(cb_ps_light.Initialize(), "Failed to Initialize cb_ps_light buffer.");

		cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		cb_ps_light.data.ambientLightStrength = 1.0f;

		auto model = Pool::CreateInstance<Model>();
		model->Initialize("Data\\Objects\\nanosuit\\nanosuit.obj");
		gameObject->GetRenderer().Model = model;
		model = Pool::CreateInstance<Model>();
		model->Initialize("Data\\Objects\\Test_cat\\12221_Cat_v1_l3.obj");
		gameObject->GetRenderer().Vshader = std::shared_ptr<VertexShader>(&vertexshader);
		gameObject->GetRenderer().Pshader = std::shared_ptr<PixelShader>(&pixelshader);
		model = Pool::CreateInstance<Model>();
		model->Initialize("Data/Objects/light.fbx");
		light->GetRenderer().Model = model;
		light->GetRenderer().Vshader = std::shared_ptr<VertexShader>(&vertexshader);
		light->GetRenderer().Pshader = std::shared_ptr<PixelShader>(&pixelshader_nolight);

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
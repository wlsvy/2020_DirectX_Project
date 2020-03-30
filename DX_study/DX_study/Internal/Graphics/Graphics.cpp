#include "Graphics.h"

#include <WICTextureLoader.h>

#include "imGui/imgui.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_impl_dx11.h"
#include "../../Util/Time.h"
#include "../DeviceResources.h"

bool Graphics::Initialize(HWND hwnd, int width, int height) {
	this->windowWidth = width;
	this->windowHeight = height;

	/*if (!InitializeDirectX(hwnd)) {
		MessageBoxA(NULL, "Initialize DX Error", "Error", MB_ICONERROR);
		return false;
	}*/

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
	ImGui_ImplDX11_Init(DeviceResources::GetInstance().GetDevice(), DeviceResources::GetInstance().GetDeviceContext());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{
	auto& deviceResources = DeviceResources::GetInstance();

	cb_ps_light.data.dynamicLightColor = light.lightColor;
	cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;
	cb_ps_light.ApplyChanges();
	deviceResources.GetDeviceContext()->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());

	static float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceResources.GetDeviceContext()->ClearRenderTargetView(deviceResources.GetBaseRenderTargetView(), bgcolor);
	deviceResources.GetDeviceContext()->ClearDepthStencilView(deviceResources.GetBaseDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceResources.GetDeviceContext()->IASetInputLayout(this->vertexshader.GetInputLayout());
	deviceResources.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceResources.GetDeviceContext()->RSSetState(deviceResources.GetRasterizerState());
	deviceResources.GetDeviceContext()->OMSetDepthStencilState(deviceResources.GetBaseDepthStencilState(), 0);

	static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	deviceResources.GetDeviceContext()->OMSetBlendState(DeviceResources::GetInstance().GetBlendState(), blendFactors, 0xFFFFFFFF);
	deviceResources.GetDeviceContext()->PSSetSamplers(0, 1, deviceResources.GetSamplerStateAddr());

	deviceResources.GetDeviceContext()->VSSetShader(vertexshader.GetShader(), NULL, 0);
	deviceResources.GetDeviceContext()->PSSetShader(pixelshader.GetShader(), NULL, 0);
	deviceResources.GetDeviceContext()->IASetInputLayout(vertexshader.GetInputLayout());

	{
		gameObject.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
	}
	{
		deviceResources.GetDeviceContext()->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		light.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
	}
	
	DrawFrameString();
	DrawImGui();
	
	deviceResources.PresentSwapChain();
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
	auto spriteBatch = DeviceResources::GetInstance().GetSpriteBatch();
	auto spriteFont = DeviceResources::GetInstance().GetSpriteFont();
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
	ImGui::DragFloat3("Ambient Light Color", &this->cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strenght", &this->cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &this->light.lightColor.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &this->light.lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &this->light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &this->light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &this->light.attenuation_c, 0.01f, 0.0f, 10.0f);
	
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//bool Graphics::InitializeDirectX(HWND hwnd)
//{
//	try{
//		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
//
//		if (adapters.size() < 1)
//		{
//			ErrorLogger::Log("No IDXGI Adapters found.");
//			return false;
//		}
//
//		DXGI_SWAP_CHAIN_DESC scd = { 0 };
//
//		scd.BufferDesc.Width = this->windowWidth;
//		scd.BufferDesc.Height = this->windowHeight;
//		scd.BufferDesc.RefreshRate.Numerator = 60;
//		scd.BufferDesc.RefreshRate.Denominator = 1;
//		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//
//		scd.SampleDesc.Count = 1;
//		scd.SampleDesc.Quality = 0;
//
//		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		scd.BufferCount = 1;
//		scd.OutputWindow = hwnd;
//		scd.Windowed = TRUE;
//		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//
//		HRESULT hr;
//		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
//			D3D_DRIVER_TYPE_UNKNOWN,
//			NULL, //FOR SOFTWARE DRIVER TYPE
//			NULL, //FLAGS FOR RUNTIME LAYERS
//			NULL, //FEATURE LEVELS ARRAY
//			0, //# OF FEATURE LEVELS IN ARRAY
//			D3D11_SDK_VERSION,
//			&scd, //Swapchain description
//			this->swapchain.GetAddressOf(), //Swapchain Address
//			this->device.GetAddressOf(), //Device Address
//			NULL, //Supported feature level
//			this->deviceContext.GetAddressOf()); //Device Context Address
//
//		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");
//
//		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
//		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
//		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");
//
//		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
//		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
//
//		//Describe our Depth/Stencil Buffer + 생성자 활용해서 코드 줄이기(애초에 default 패러미터 값은 안 건드려도 됨)
//		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
//		depthStencilDesc.MipLevels = 1;
//		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//
//		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
//		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");
//
//		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
//		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
//
//		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
//
//		//Create depth stencil state 스텐실 & 뎁스
//		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
//		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
//
//		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
//		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
//
//		//뷰포트 만들기 & 세팅
//		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
//		this->deviceContext->RSSetViewports(1, &viewport);
//
//		//래스터라이저 state 설정
//		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
//		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
//		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
//
//		//Create Blend state
//		D3D11_BLEND_DESC blendDesc = { 0 };
//		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
//
//		rtbd.BlendEnable = true;
//		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
//		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
//		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
//		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
//		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
//		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD; //Blend operation
//		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
//
//		blendDesc.RenderTarget[0] = rtbd;
//
//		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
//		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");
//
//		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
//		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");
//
//		//Create sampler description for sampler state
//		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
//		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
//		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
//	}
//	catch (COMException & exception) {
//		ErrorLogger::Log(exception);
//		return false;
//	}
//
//	return true;
//}

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
		//텍스쳐 불러오기
		HRESULT hr = DirectX::CreateWICTextureFromFile(Core::GetDevice(), L"Data\\Textures\\test.png", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//Initialize Constant buffer(s)
		hr = this->cb_vs_vertexshader.Initialize();
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize constant buffer.");

		hr = this->cb_vs_vertexshader_2d.Initialize();
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize 2d constant buffer.");

		hr = this->cb_ps_light.Initialize();
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize constant buffer.");

		this->cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		this->cb_ps_light.data.ambientLightStrength = 1.0f;

		//모델 데이터 초기화
		if (!gameObject.Initialize("Data\\Objects\\nanosuit\\nanosuit.obj", Core::GetDevice(), Core::GetDeviceContext(), this->cb_vs_vertexshader)) {
			MessageBoxA(NULL, "Initialize Model Error", "Error", MB_ICONERROR);
			return false;
		}

		if (!gameObject.Initialize("Data\\Objects\\Test_cat\\12221_Cat_v1_l3.obj", Core::GetDevice(), Core::GetDeviceContext(), this->cb_vs_vertexshader)) {
			MessageBoxA(NULL, "Initialize Model Error", "Error", MB_ICONERROR);
			return false;
		}

		//광원 데이터 초기화
		if (!light.Initialize(Core::GetDevice(), Core::GetDeviceContext(), this->cb_vs_vertexshader)) {
			MessageBoxA(NULL, "Initialize light Error", "Error", MB_ICONERROR);
			return false;
		}

		//스프라이트 생성
		if (!sprite.Initialize(Core::GetDevice(), Core::GetDeviceContext(), 256, 256, "Data/Textures/circle.png", cb_vs_vertexshader_2d))
		{
			MessageBoxA(NULL, "Initialize sprite Error", "Error", MB_ICONERROR);
			return false;
		}
		//sprite.SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		sprite.SetPosition(XMFLOAT3(windowWidth / 2 - sprite.GetWidth()/2, windowHeight / 2 - sprite.GetHeight()/2, 0.0f));

		Camera2D.SetProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);

		Camera3D.SetPosition(0.0f, 0.0f, -2.0f);
		Camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}
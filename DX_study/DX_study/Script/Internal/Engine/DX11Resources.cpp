#include "DX11Resources.h"
#include "../Core/InternalHelper.h"



bool DX11Resources::Initialize(HWND hwnd, UINT width, UINT height)
{
	try {
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			StringHelper::ErrorLog("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = width;
		scd.BufferDesc.Height = height;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			D3D11_CREATE_DEVICE_DEBUG, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			this->m_Swapchain.GetAddressOf(), //Swapchain Address
			this->m_Device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->m_DeviceContext.GetAddressOf()); //Device Context Address

		ThrowIfFailed(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		ThrowIfFailed(hr, "GetBuffer Failed.");

		hr = this->m_Device->CreateRenderTargetView(backBuffer.Get(), NULL, this->m_MainRenderTargetView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create render target view.");

		//뷰포트 만들기 & 세팅
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
		m_DeviceContext->RSSetViewports(1, &viewport);

		//Describe our Depth/Stencil Buffer + 생성자 활용해서 코드 줄이기(애초에 default 패러미터 값은 안 건드려도 됨)
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_R32_TYPELESS, width, height);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer, depthStencilBuffer2;
		hr = this->m_Device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil buffer.");
		hr = this->m_Device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer2.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil buffer.");

		CD3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		descDSV.Flags = 0;
		hr = this->m_Device->CreateDepthStencilView(depthStencilBuffer.Get(), &descDSV, this->m_MainDepthStencilView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil view.");
		hr = this->m_Device->CreateDepthStencilView(depthStencilBuffer2.Get(), &descDSV, this->m_SubDepthStencilView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil view.");

		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; 
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		ThrowIfFailed(
			m_Device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, m_MainDepthStencilSRV.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");
		ThrowIfFailed(
			m_Device->CreateShaderResourceView(depthStencilBuffer2.Get(), &shaderResourceViewDesc, m_SubDepthStencilSRV.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");

		////Create depth stencil state 스텐실 & 뎁스
		//CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		//depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		//hr = this->m_Device->CreateDepthStencilState(&depthstencildesc, this->m_DepthStencilState.GetAddressOf());
		//ThrowIfFailed(hr, "Failed to create depth stencil state.");

		CreateDepthStencilState(m_DepthStencilState.GetAddressOf());

		CreateRasterizerState(m_RasterizerState.GetAddressOf());

		CreateBlenderState(m_BlendState.GetAddressOf(), true, D3D11_BLEND::D3D11_BLEND_SRC_ALPHA, D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA);

		CreateSamplerState(m_SamplerState.GetAddressOf());

		m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(m_DeviceContext.Get());
		m_SpriteFont = std::make_unique<DirectX::SpriteFont>(m_Device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");
		m_CommonState = std::make_unique<DirectX::CommonStates>(m_Device.Get());
		m_BasicEffect = std::make_unique<DirectX::BasicEffect>(m_Device.Get());
		m_DebugEffect = std::make_unique<DirectX::DebugEffect>(m_Device.Get());
		m_PrimitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_DeviceContext.Get());


	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
		return false;
	}

	return true;
}

bool DX11Resources::InitializeRenderTarget(UINT width, UINT height)
{
	try {
		UINT widthArr[RenderTargetCount] = { width , width , width , width  ,width,  width, width / 2, width / 2 };
		UINT heightArr[RenderTargetCount] = { height , height , height , height  ,height,  height, height / 2 , height / 2 };
		UINT formatArr[RenderTargetCount] = { 2, 2, 2, 2, 2, 2, 28, 28 };
		for (int i = 0; i < RenderTargetCount; i++) {
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = widthArr[i];
			textureDesc.Height = heightArr[i];
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = (DXGI_FORMAT)formatArr[i];
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			ThrowIfFailed(
				m_Device->CreateTexture2D(&textureDesc, NULL, m_RenderTargetTextureArr[i].GetAddressOf()),
				"Failed to create renderTargetTextureArr.");

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(
				m_Device->CreateRenderTargetView(m_RenderTargetTextureArr[i].Get(), &renderTargetViewDesc, m_RenderTargetViewArr[i].GetAddressOf()),
				"Failed to create renderTargetViewArr.");

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			ThrowIfFailed(
				m_Device->CreateShaderResourceView(m_RenderTargetTextureArr[i].Get(), &shaderResourceViewDesc, m_ShaderResourceViewArr[i].GetAddressOf()),
				"Failed to create shaderResourceViewArr.");

			D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
			ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));
			unorderedAccessViewDesc.Format = textureDesc.Format;
			unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			unorderedAccessViewDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(
				Core::GetDevice()->CreateUnorderedAccessView(m_RenderTargetTextureArr[i].Get(), &unorderedAccessViewDesc, m_UnorderedAccessView[i].GetAddressOf()),
				"Failed to create UnorderedAccessView.");
		}
	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
		return false;
	}

	return true;
}

void DX11Resources::CreateRasterizerState(
	ID3D11RasterizerState** addr,
	D3D11_FILL_MODE fillMode,
	D3D11_CULL_MODE cullMode)
{
	CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);
	desc.FillMode = fillMode;
	desc.CullMode = cullMode;
	ThrowIfFailed(
		m_Device->CreateRasterizerState(&desc, addr),
		"Failed to create rasterizer state.");
}

void DX11Resources::CreateDepthStencilState(ID3D11DepthStencilState** addr)
{
	CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	ThrowIfFailed(
		m_Device->CreateDepthStencilState(&depthstencildesc, addr),
		"Failed to create depth stencil state.");
}

void DX11Resources::CreateSamplerState(
	ID3D11SamplerState ** addr,
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressMode) 
{
	CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
	desc.Filter = filter;
	desc.AddressU = addressMode;
	desc.AddressV = addressMode;
	desc.AddressW = addressMode;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 1.0f;
	ThrowIfFailed(
		m_Device->CreateSamplerState(&desc, addr),
		"Failed to create sampler state.");
}

void DX11Resources::CreateBlenderState(
	ID3D11BlendState ** addr,
	bool blendEnable,
	D3D11_BLEND SrcBlend,
	D3D11_BLEND DestBlend)
{
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = blendEnable;
	for (auto& rtDesc : desc.RenderTarget) {
		rtDesc.BlendEnable = blendEnable;
		rtDesc.SrcBlend = SrcBlend;
		rtDesc.DestBlend = DestBlend;
		rtDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	ThrowIfFailed(
		m_Device->CreateBlendState(&desc, addr),
		"Failed to create blend state.");
}

bool DX11Resources::InitializeDebugLayout(DirectX::XMMATRIX v, DirectX::XMMATRIX p)
{
	try {
		void const* shaderByteCode;
		size_t byteCodeLength;
		m_BasicEffect->SetVertexColorEnabled(true);
		m_BasicEffect->SetView(v);
		m_BasicEffect->SetProjection(p);
		m_BasicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		ThrowIfFailed(
			m_Device->CreateInputLayout(
				DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
				shaderByteCode, byteCodeLength,
				m_DebugInputLayout.GetAddressOf()),
			"Failed to create debug input layout.");
	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
		return false;
	}

	return true;
	
}
#include "DeviceResources.h"

bool DeviceResources::Initialize(HWND hwnd, int width, int height)
{
	try {
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
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
			this->swapchain.GetAddressOf(), //Swapchain Address
			this->device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->deviceContext.GetAddressOf()); //Device Context Address

		ThrowIfFailed(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		ThrowIfFailed(hr, "GetBuffer Failed.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->mainRenderTargetView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create render target view.");

		//Describe our Depth/Stencil Buffer + 생성자 활용해서 코드 줄이기(애초에 default 패러미터 값은 안 건드려도 됨)
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil buffer.");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil view.");

		this->deviceContext->OMSetRenderTargets(1, this->mainRenderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create depth stencil state 스텐실 & 뎁스
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil state.");

		//뷰포트 만들기 & 세팅
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
		this->deviceContext->RSSetViewports(1, &viewport);

		//래스터라이저 state 설정
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create rasterizer state.");

		//Create Blend state
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD; //Blend operation
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtbd;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");
		commonState = std::make_unique<DirectX::CommonStates>(device.Get());
		basicEffect = std::make_unique<DirectX::BasicEffect>(device.Get());
		debugEffect = std::make_unique<DirectX::DebugEffect>(device.Get());
		primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(deviceContext.Get());

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
		ThrowIfFailed(hr, "Failed to create sampler state.");
	}
	catch (CustomException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool DeviceResources::InitializeRenderTarget(int width, int height)
{
	//렌더타겟 추가하기
	try {
		for (int i = 0; i < RenderTargetCount; i++) {
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			ThrowIfFailed(
				device->CreateTexture2D(&textureDesc, NULL, renderTargetTextureArr[i].GetAddressOf()),
				"Failed to create renderTargetTextureArr.");

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(
				device->CreateRenderTargetView(renderTargetTextureArr[i].Get(), &renderTargetViewDesc, renderTargetViewArr[i].GetAddressOf()),
				"Failed to create renderTargetViewArr.");

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			shaderResourceViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			ThrowIfFailed(
				device->CreateShaderResourceView(renderTargetTextureArr[i].Get(), &shaderResourceViewDesc, shaderResourceViewArr[i].GetAddressOf()),
				"Failed to create shaderResourceViewArr.");
		}

		/*D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		for (int i = 0; i < RenderTargetCount; i++) {
			ThrowIfFailed(
				device->CreateTexture2D(&textureDesc, NULL, renderTargetTextureArr[i].GetAddressOf()),
				"Failed to create renderTargetTextureArr.");
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		for (int i = 0; i < RenderTargetCount; i++) {
			ThrowIfFailed(
				device->CreateRenderTargetView(renderTargetTextureArr[i].Get(), &renderTargetViewDesc, renderTargetViewArr[i].GetAddressOf()),
				"Failed to create renderTargetViewArr.");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		for (int i = 0; i < RenderTargetCount; i++) {
			ThrowIfFailed(
				device->CreateShaderResourceView(renderTargetTextureArr[i].Get(), &shaderResourceViewDesc, shaderResourceViewArr[i].GetAddressOf()),
				"Failed to create shaderResourceViewArr.");
		}*/
	}
	catch (CustomException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

void DeviceResources::SetDebugLayout(DirectX::XMMATRIX v, DirectX::XMMATRIX p)
{
	void const* shaderByteCode;
	size_t byteCodeLength;
	basicEffect->SetVertexColorEnabled(true);
	basicEffect->SetView(v);
	basicEffect->SetProjection(p);
	basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	ThrowIfFailed(
		device->CreateInputLayout(
			DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			debugInputLayout.GetAddressOf()),
		"Failed to create debug input layout.");
}

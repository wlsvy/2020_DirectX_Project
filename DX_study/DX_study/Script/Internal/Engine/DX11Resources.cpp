#include "DX11Resources.h"
#include "../Core/InternalHelper.h"
#include "../Engine/Profiler.h"

#include <array>



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

		//Describe our Depth/Stencil Buffer
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

		CreateDepthStencilState(m_DepthStencilState.GetAddressOf());

		CreateRasterizerState(m_RasterizerCullBack.GetAddressOf(), D3D11_FILL_SOLID, D3D11_CULL_BACK);
		CreateRasterizerState(m_RasterizerCullFront.GetAddressOf(), D3D11_FILL_SOLID, D3D11_CULL_FRONT);
		CreateRasterizerState(m_RasterizerCullNone.GetAddressOf(), D3D11_FILL_SOLID, D3D11_CULL_NONE);

		CreateBlenderState(m_BlendStateOpaque.GetAddressOf(), false);
		CreateBlenderState(m_BlendStateAlpha.GetAddressOf(), true, D3D11_BLEND::D3D11_BLEND_SRC_ALPHA, D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND::D3D11_BLEND_ONE, D3D11_BLEND::D3D11_BLEND_ONE);
		CreateBlenderState(m_BlendStateAdditive.GetAddressOf(), true, D3D11_BLEND::D3D11_BLEND_ONE, D3D11_BLEND::D3D11_BLEND_ONE, D3D11_BLEND::D3D11_BLEND_ONE, D3D11_BLEND::D3D11_BLEND_ONE);

		CreateSamplerState(m_SamplerPointClamp.GetAddressOf(), D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
		CreateSamplerState(m_SamplerLinearClamp.GetAddressOf(), D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
		CreateSamplerState(m_SamplerLinearWrap.GetAddressOf(), D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
		CreateSamplerState(m_SamplerLinearMirror.GetAddressOf(), D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_MIRROR);
		CreateSamplerState(m_SamplerAnisotropicWrap.GetAddressOf(), D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
		CreateSamplerState(m_SamplerTrilinearWrap.GetAddressOf(), D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
		
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::DeferredRenderingResource0].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource0].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::DeferredRenderingResource0].GetAddressOf(), width, height);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::DeferredRenderingResource1].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource1].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::DeferredRenderingResource1].GetAddressOf(), width, height);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::DeferredRenderingResource2].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource2].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::DeferredRenderingResource2].GetAddressOf(), width, height);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::DeferredRenderingResource3].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource3].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::DeferredRenderingResource3].GetAddressOf(), width, height);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::Composition0].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::Composition0].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::Composition0].GetAddressOf(), width, height);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::Composition1].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::Composition1].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::Composition1].GetAddressOf(), width, height);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::BlurIn].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::BlurIn].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::BlurIn].GetAddressOf(), width * 0.5, height * 0.5, DXGI_FORMAT_R8G8B8A8_UNORM);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::BlurOut].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::BlurOut].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::BlurOut].GetAddressOf(), width * 0.5, height * 0.5, DXGI_FORMAT_R8G8B8A8_UNORM);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::HalfSize].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::HalfSize].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::HalfSize].GetAddressOf(), width * 0.5, height * 0.5, DXGI_FORMAT_R8G8B8A8_UNORM);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::QuarterSize].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::QuarterSize].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::QuarterSize].GetAddressOf(), width * 0.25, height * 0.25, DXGI_FORMAT_R8G8B8A8_UNORM);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::HalfQuarterSize].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::HalfQuarterSize].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::HalfQuarterSize].GetAddressOf(), width * 0.125, height * 0.125, DXGI_FORMAT_R8G8B8A8_UNORM);
		CreateRenderTarget(m_RenderTargetViewArr[RenderTargetTypes::SSAO].GetAddressOf(), m_RenderTargetSrvs[RenderTargetTypes::SSAO].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::SSAO].GetAddressOf(), width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

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
	ThrowIfFailed(
		m_Device->CreateSamplerState(&desc, addr),
		"Failed to create sampler state.");
}

void DX11Resources::CreateBlenderState(
	ID3D11BlendState ** addr,
	bool blendEnable,
	D3D11_BLEND SrcBlend,
	D3D11_BLEND DestBlend,
	D3D11_BLEND srcBlendAlpha,
	D3D11_BLEND destBlendAlpha)
{
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = blendEnable;
	for (auto& rtDesc : desc.RenderTarget) {
		rtDesc.BlendEnable = blendEnable;
		rtDesc.SrcBlend = SrcBlend;
		rtDesc.DestBlend = DestBlend;
		rtDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = srcBlendAlpha;
		rtDesc.DestBlendAlpha = destBlendAlpha;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	ThrowIfFailed(
		m_Device->CreateBlendState(&desc, addr),
		"Failed to create blend state.");
}

void DX11Resources::CreateRenderTarget(
	ID3D11RenderTargetView ** rtv, 
	ID3D11ShaderResourceView ** srv, 
	ID3D11UnorderedAccessView ** uav, 
	UINT widht, 
	UINT height, 
	DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = widht;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	ThrowIfFailed(
		m_Device->CreateTexture2D(&textureDesc, NULL, texture.GetAddressOf()),
		"Failed to create renderTargetTextureArr.");

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		m_Device->CreateRenderTargetView(texture.Get(), &renderTargetViewDesc, rtv),
		"Failed to create renderTargetViewArr.");

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	ThrowIfFailed(
		m_Device->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, srv),
		"Failed to create shaderResourceViewArr.");

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));
	unorderedAccessViewDesc.Format = textureDesc.Format;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	unorderedAccessViewDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		Core::GetDevice()->CreateUnorderedAccessView(texture.Get(), &unorderedAccessViewDesc, uav),
		"Failed to create UnorderedAccessView.");
}

void DX11Resources::SetPsSampler(UINT startSlot, ID3D11SamplerState ** sampler)
{
	ID3D11SamplerState* prev = nullptr;
	m_DeviceContext->PSGetSamplers(startSlot, 1, &prev);

	if (prev == sampler[0]) {
		return;
	}

	m_DeviceContext->PSSetSamplers(startSlot, 1, sampler);

	Profiler::GetInstance().SamplerStateBindingCount++;
}

void DX11Resources::SetVertexBuffer(ID3D11Buffer * const * vertexBuffer, const UINT * stridePtr)
{
	ID3D11Buffer* prev = nullptr;
	UINT prevStride = 0;
	UINT offset = 0;
	m_DeviceContext->IAGetVertexBuffers(0, 1, &prev, &prevStride, &offset);

	if (vertexBuffer[0] == prev) {
		return;
	}

	offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer, stridePtr, &offset);
	Profiler::GetInstance().VertexBufferBindingCount++;
}

void DX11Resources::SetIndexBuffer(ID3D11Buffer * indexBuffer)
{
	ID3D11Buffer* prev = nullptr;
	DXGI_FORMAT prevFormat;
	UINT offset = 0;
	m_DeviceContext->IAGetIndexBuffer(&prev, &prevFormat, &offset);

	if (indexBuffer == prev) {
		return;
	}

	offset = 0;
	m_DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	Profiler::GetInstance().IndexBufferBindingCount++;
}

void DX11Resources::SetVSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer)
{
	ID3D11Buffer* prev = nullptr;
	m_DeviceContext->VSGetConstantBuffers(startSlot, 1, &prev);

	if (prev == buffer[0]) {
		return;
	}

	m_DeviceContext->VSSetConstantBuffers(startSlot, 1, buffer);

	Profiler::GetInstance().ConstantBufferBindingCount++;
}

void DX11Resources::SetVSInputLayout(ID3D11InputLayout * inputLayout)
{
	ID3D11InputLayout * prev;
	m_DeviceContext->IAGetInputLayout(&prev);

	if (prev == inputLayout) {
		return;
	}

	m_DeviceContext->IASetInputLayout(inputLayout);
}

void DX11Resources::SetVertexShader(ID3D11VertexShader * shader)
{
	ID3D11VertexShader* prevShader = nullptr;
	ID3D11ClassInstance* prevInstance = nullptr;
	UINT instanceNum = 0;
	m_DeviceContext->VSGetShader(&prevShader, &prevInstance, &instanceNum);

	if (shader != prevShader) {
		m_DeviceContext->VSSetShader(shader, NULL, 0);
		Profiler::GetInstance().VertexShaderBindingCount++;
	}
}

void DX11Resources::SetPSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer)
{
	ID3D11Buffer* prev = nullptr;
	m_DeviceContext->PSGetConstantBuffers(startSlot, 1, &prev);

	if (prev == buffer[0]) {
		return;
	}

	m_DeviceContext->PSSetConstantBuffers(startSlot, 1, buffer);

	Profiler::GetInstance().ConstantBufferBindingCount++;
}

void DX11Resources::SetPixelShader(ID3D11PixelShader * shader)
{
	ID3D11PixelShader* prevShader = nullptr;
	ID3D11ClassInstance* prevInstance = nullptr;
	UINT instanceNum = 0;
	m_DeviceContext->PSGetShader(&prevShader, &prevInstance, &instanceNum);

	if (shader != prevShader) {
		m_DeviceContext->PSSetShader(shader, NULL, 0);
		Profiler::GetInstance().PixelShaderBindingCount++;
	}
}

void DX11Resources::SetPSShaderResources(UINT startSlot, UINT range, ID3D11ShaderResourceView *const* srv)
{
	std::array<ID3D11ShaderResourceView*, MAX_SHADER_RESOURCE_VIEW_BINDING_COUNT> prev = { nullptr };
	m_DeviceContext->PSGetShaderResources(startSlot, range, prev.data());

	std::array<ID3D11ShaderResourceView*, MAX_SHADER_RESOURCE_VIEW_BINDING_COUNT> srvArr;
	for (UINT i = 0; i < range; i++) {
		srvArr[i] = srv[i];
	}

	if (srvArr == prev) {
		return;
	}

	m_DeviceContext->PSSetShaderResources(startSlot, range, srv);
	Profiler::GetInstance().ShaderResourcesBindingCount++;
}

void DX11Resources::SetGSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer)
{
	ID3D11Buffer* prev = nullptr;
	m_DeviceContext->GSGetConstantBuffers(startSlot, 1, &prev);

	if (prev == buffer[0]) {
		return;
	}

	m_DeviceContext->GSSetConstantBuffers(startSlot, 1, buffer);

	Profiler::GetInstance().ConstantBufferBindingCount++;
}

void DX11Resources::SetGeometryShader(ID3D11GeometryShader * shader)
{
	ID3D11GeometryShader* prevShader = nullptr;
	ID3D11ClassInstance* prevInstance = nullptr;
	UINT instanceNum = 0;
	m_DeviceContext->GSGetShader(&prevShader, &prevInstance, &instanceNum);

	if (shader != prevShader) {
		m_DeviceContext->GSSetShader(shader, NULL, 0);
		Profiler::GetInstance().GeometryShaderBindingCount++;
	}
}

void DX11Resources::SetComputeShader(ID3D11ComputeShader * shader)
{
	ID3D11ComputeShader* prevShader = nullptr;
	ID3D11ClassInstance* prevInstance = nullptr;
	UINT instanceNum = 0;
	m_DeviceContext->CSGetShader(&prevShader, &prevInstance, &instanceNum);

	if (shader != prevShader) {
		m_DeviceContext->CSSetShader(shader, NULL, 0);
		Profiler::GetInstance().ComputeShaderBindingCount++;
	}
}

void DX11Resources::SetCSConstantBuffer(UINT startSlot, ID3D11Buffer * const * buffer)
{
	ID3D11Buffer* prev = nullptr;
	m_DeviceContext->CSGetConstantBuffers(startSlot, 1, &prev);

	if (prev == buffer[0]) {
		return;
	}

	m_DeviceContext->CSSetConstantBuffers(startSlot, 1, buffer);

	Profiler::GetInstance().ConstantBufferBindingCount++;
}

void DX11Resources::SetCSShaderResources(UINT startSlot, UINT range, ID3D11ShaderResourceView * const * srv)
{
	std::array<ID3D11ShaderResourceView*, MAX_SHADER_RESOURCE_VIEW_BINDING_COUNT> prev = { nullptr };
	m_DeviceContext->CSGetShaderResources(startSlot, range, prev.data());

	std::array<ID3D11ShaderResourceView*, MAX_SHADER_RESOURCE_VIEW_BINDING_COUNT> srvArr;
	for (UINT i = 0; i < range; i++) {
		srvArr[i] = srv[i];
	}

	if (srvArr == prev) {
		return;
	}

	m_DeviceContext->CSSetShaderResources(startSlot, range, srv);
	Profiler::GetInstance().ShaderResourcesBindingCount++;
}

void DX11Resources::SetCSUavResources(UINT startSlot, UINT range, ID3D11UnorderedAccessView * const * uav)
{
	std::array<ID3D11UnorderedAccessView*, MAX_UNORDERED_ACCESS_VIEW_BINDING_COUNT> prev = { nullptr };
	m_DeviceContext->CSGetUnorderedAccessViews(startSlot, range, prev.data());

	std::array<ID3D11UnorderedAccessView*, MAX_UNORDERED_ACCESS_VIEW_BINDING_COUNT> uavArr;
	for (UINT i = 0; i < range; i++) {
		uavArr[i] = uav[i];
	}

	if (uavArr == prev) {
		return;
	}

	m_DeviceContext->CSSetUnorderedAccessViews(startSlot, range, uav, nullptr);
	Profiler::GetInstance().UnorderedAccessViewBindingCount++;
}

void DX11Resources::SetBlendState(ID3D11BlendState * blendState, const float * factor)
{
	ID3D11BlendState *prevBlendState = nullptr;
	float *prevBlendFactor = nullptr;
	UINT prevMask = 0;
	m_DeviceContext->OMGetBlendState(&prevBlendState, prevBlendFactor, &prevMask);

	if (blendState != prevBlendState || factor != prevBlendFactor) {
		m_DeviceContext->OMSetBlendState(blendState, factor, 0xFFFFFFFF);
	}
}

void DX11Resources::SetDepthStencilState(ID3D11DepthStencilState * state)
{
	ID3D11DepthStencilState* prev = nullptr;
	UINT prevStencilRef = 0;
	m_DeviceContext->OMGetDepthStencilState(&prev, &prevStencilRef);

	if (state != prev) {
		m_DeviceContext->OMSetDepthStencilState(state, 0);
	}
}

void DX11Resources::SetRasterizerState(ID3D11RasterizerState * rasterizer)
{
	ID3D11RasterizerState* prev = nullptr;
	m_DeviceContext->RSGetState(&prev);
	
	if (rasterizer != prev) {
		m_DeviceContext->RSSetState(rasterizer);
	}
}

void DX11Resources::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	D3D11_PRIMITIVE_TOPOLOGY prev;
	m_DeviceContext->IAGetPrimitiveTopology(&prev);

	if (topology != prev) {
		m_DeviceContext->IASetPrimitiveTopology(topology);
	}
}

void DX11Resources::SetRenderTarget(
	UINT numViews, 
	ID3D11RenderTargetView * const * renderTargetView, 
	ID3D11DepthStencilView * depthStencilView)
{
	std::array<ID3D11RenderTargetView*, MAX_RENDER_TARGET_BINDING_COUNT> prevRtv = { nullptr };
	ID3D11DepthStencilView* prevDsv = nullptr;
	m_DeviceContext->OMGetRenderTargets(MAX_RENDER_TARGET_BINDING_COUNT, prevRtv.data(), &prevDsv);

	std::array<ID3D11RenderTargetView*, MAX_RENDER_TARGET_BINDING_COUNT> rtvArr = { nullptr };
	for (UINT i = 0; i < numViews; i++) {
		rtvArr[i] = renderTargetView[i];
	}

	if (rtvArr != prevRtv || depthStencilView != prevDsv)
	{
		m_DeviceContext->OMSetRenderTargets
		(
			numViews,
			renderTargetView,
			depthStencilView
		);

		Profiler::GetInstance().RenderTargetBindingCount++;
	}
}

void DX11Resources::DrawIndexed(const UINT indexCount)
{
	m_DeviceContext->DrawIndexed(indexCount, 0, 0);
	Profiler::GetInstance().DrawCallCount++;
}

void DX11Resources::DispatchComputeShader(const UINT X, const UINT Y, const UINT Z)
{
	m_DeviceContext->Dispatch(X, Y, Z);
	Profiler::GetInstance().DispatchCallCount++;
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
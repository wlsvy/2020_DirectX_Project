#include "Graphics.h"
#include <algorithm>

#include "../Engine/ModuleResource.h"
#include "../Component/BaseComponentInclude.h"
#include "../Component/ScriptBehaviour.h"
#include "BaseGeometry.h"
#include "../Engine.h"
#include "../UI/ui.h"

#define Deg2Rad 0.0174533	// pi / 180
#define Rad2Deg 57.2958		// 180 / pi



void GraphicsManager::RegisterComponent(const std::shared_ptr<Terrain>& compo)
{
	m_Terrains.push_back(compo);
}

void GraphicsManager::RegisterComponent(const std::shared_ptr<Light_ver2>& compo)
{
	m_Lights.push_back(compo);
}

void GraphicsManager::DeregisterComponent(const std::shared_ptr<Terrain>& compo)
{
	auto iter = std::find(m_Terrains.begin(), m_Terrains.end(), compo);

	if (iter != m_Terrains.end()) {
		m_Terrains.erase(iter);
	}
}

void GraphicsManager::DeregisterComponent(const std::shared_ptr<Light_ver2>& compo)
{
	auto iter = std::find(m_Lights.begin(), m_Lights.end(), compo);

	if (iter != m_Lights.end()) {
		m_Lights.erase(iter);
	}
}

VertexShader* GraphicsManager::GetVshader(const std::string & shaderName)
{
	auto iter = m_VshaderBuffer.find(shaderName);
	if (iter != m_VshaderBuffer.end()) {
		return iter->second.get();
	}
	return nullptr;
}

PixelShader* GraphicsManager::GetPshader(const std::string & shaderName)
{
	auto iter = m_PshaderBuffer.find(shaderName);
	if (iter != m_PshaderBuffer.end()) {
		return iter->second.get();
	}
	return nullptr;
}

GeometryShader* GraphicsManager::GetGshader(const std::string & shaderName)
{
	auto iter = m_GshaderBuffer.find(shaderName);
	if (iter != m_GshaderBuffer.end()) {
		return iter->second.get();
	}
	return nullptr;
}


#pragma region Method - Initialize

bool GraphicsManager::Initialize(HWND hwnd, int width, int height) {
	this->windowWidth = width;
	this->windowHeight = height;

	if (!InitializeDirectX(hwnd)) {
		MessageBoxA(NULL, "Initialize DX Error", "Error", MB_ICONERROR);
		return false;
	}

	if (!Initialize_MoreRenderTarget()) {
		MessageBoxA(NULL, "Initialize Additional Rendertarget Error", "Error", MB_ICONERROR);
		return false;
	}

	if (!InitializeShaders()) {
		MessageBoxA(NULL, "Initialize Shader Error", "Error", MB_ICONERROR);
		return false;
	}

	if (!InitializeTextures()) {
		MessageBoxA(NULL, "Initialize Texture Error", "Error", MB_ICONERROR);
		return false;
	}

	if (!Initialize_Skybox()) {
		MessageBoxA(NULL, "Initialize Skybox Error", "Error", MB_ICONERROR);
		return false;
	}

	/*if (!InitializeScene()) {
		MessageBoxA(NULL, "Initialize Scene Error", "Error", MB_ICONERROR);
		return false;
	}*/
		
	InitializeDebugDraw();

	UIspace::InitImGUI(m_Device.Get(), m_DeviceContext.Get(), hwnd);

	return true;
}

bool GraphicsManager::InitializeDirectX(HWND hwnd)
{
	try {
		HRESULT hr;

		//����� �ҷ�����
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		//******************************
		//******���� ü�� ����ü ����
		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = this->windowWidth; //â ����
		scd.BufferDesc.Height = this->windowHeight; //â �ʺ�
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//SampleDesc ���� ǥ��ȭMulti sampling �� ���� ������ ǥ�� ������ ǰ�� ���� 
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //���� �뵵 -> DXGI_USAGE_RENDER_TARGET_OUTPUT �ĸ� ���ۿ� ������
		scd.BufferCount = 1; //�ĸ� ���� ���� -> 1�� �ĸ� ���� �Ѱ�(���� ���۸�)
		scd.OutputWindow = hwnd; //������ ����� ���� �ڵ�
		scd.Windowed = TRUE; // â��弳��
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //��ȯ ȿ�� ���� -> DXGI_SWAP_EFFECT_DISCARD ���÷��� �����Ⱑ ���� ȿ������ ���� ���
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		//�߰� �÷��� -> DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH 
		//���� ���α׷��� ��üȭ�� ���� ��ȯ�� �� ������ �ĸ� ���� ������ ���� �� �����ϴ� ���÷��� ��尡 �ڵ����� ����


		//����� �÷��� ����
		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		//******************************
		hr = D3D11CreateDeviceAndSwapChain(
			adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			createDeviceFlags, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION, //����ϴ� ����, ������ D3D11_SDK_VERSION ���
			&scd, //Swapchain description
			this->m_SwapChain.GetAddressOf(), //Swapchain Address
			this->m_Device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->m_DeviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		//MSAA Multisample anti-aliasing ����
		UINT m4xMsaaQuality;
		hr = this->m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
		//�־��� �ؽ��� ������ ǥ�������� ���տ� ���� ǰ�� ���ص��� ����
		COM_ERROR_IF_FAILED(hr, "Failed to set Msaa QualityLevel.");
		/*����ǥ��ȭ ���� ����ü
		typedef struct DXGI_SAMPLE_DESC
		{
			UINT Count;		//�ȼ��� ������ ǥ�� ���� ����
			UINT Quality;	//���ϴ� ǰ�� ����
		} DXGI_SAMPLE_DESC;
		*/

		//******************************
		//******Render target view ����
		//�ڿ��� ���������� �ܰ迡 ���� ���� ���� �ƴ� �ڿ��� ���� �並 �����Ͽ� �� �並 ���������ο� ���´�.
		//�ĸ� ���۸� ������������ ��� ���ձ�output merger �ܰ迡 ���� -> Direct3D�� ���ۿ� ������ �׸� �� �ְ� ��

		//Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer; //�ĸ� ����
		hr = this->m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_BackBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		/* GetBuffer �޼ҵ�� swapchain�� ����Ű�� �����͸� �����´�.
		virtual HRESULT STDMETHODCALLTYPE GetBuffer(
			UINT Buffer,					//����� �ϴ� �ĸ� ������ ����(�ĸ� ���۰� ���� �� �ִ� ��� �߿�. ������ �ϳ��ϱ� 0)
			_In_  REFIID riid,				//������ �������̽� ���� ����. �Ϲ������δ� �׻� 2���� �ؽ��ĸ� ���� ID3D11Texture2D�� ����
			_COM_Outptr_  void **ppSurface) = 0;	//�ĸ� ���۸� ����Ű�� �����͸� ������
		*/

		hr = this->m_Device->CreateRenderTargetView(m_BackBuffer.Get(), NULL, this->m_RenderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		/* ���� �並 �����ϴ� �޼ҵ�
		virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(
			_In_  ID3D11Resource *pResource,							//���� ������� ����� �ڿ�. �ĸ� ���� ����.
			_In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,		//�ڿ��� ��� ���ҵ��� �ڷ� ���� ����.
			_COM_Outptr_opt_  ID3D11RenderTargetView **ppRTView) = 0;	//������ ���� ��� �並 �ǵ�����
		*/

		//******************************
		//******Describe our Depth/Stencil Buffer
		//���� ���۴� �׳� ���� ������(���ٽǵ� ��������) ��� 2���� �ؽ���.
		//2���� �ؽ��ĸ� ������ ���� �Ʒ� ����ü Ȱ��.  + ������ Ȱ���ؼ� �ڵ� ���̱�(���ʿ� default �з����� ���� �� �ǵ���� ��)
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilDesc.MipLevels = 1; 
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		/*if (mEnable4xMSAA) {
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
		}
		else {
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}*/

		/*
		typedef struct D3D11_TEXTURE2D_DESC
		{
		UINT Width;		//���� �ʺ�
		UINT Height;
		UINT MipLevels; //�Ӹ� ������ ����, ����/���ٽ� ���۸� ���� �ؽ��Ŀ����� �ϳ��� ���
		UINT ArraySize;	//�ؽ�ó �迭�� �ؽ��� ����, ����/���ٽ� ������ ��� �ϳ��� ���
		DXGI_FORMAT Format;	//�ؼ��� ����. �� ��� DXGI_FORMAT_D24_UNORM_S8_UINT ��� -> ���� 24��Ʈ [0, 1]����, ���ٽ� 8��Ʈ ��ȣ ���� ����
		DXGI_SAMPLE_DESC SampleDesc;	//���� ǥ�� ������ ǰ�� ������ ���� -> ��Ƽ �ٸ����
		D3D11_USAGE Usage;				//�ؽ����� �뵵. ������ D3D11_USAGE_DEFAULT -> cpu�� �� �ڿ��� �� �� ���� gpu�� ���/�۾� ����
		UINT BindFlags;				//�ڿ��� ���������ο� ��� ���� ������ ����. ������ D3D11_BIND_DEPTH_STENCIL -> ����/���ٽ� ���� ����
		UINT CPUAccessFlags;		//cpu�� �ڿ��� �����ϴ� ����� �����ϴ� �÷���. ���� cpu�� �ڿ��� �������� �����Ƿ� 0����.
		UINT MiscFlags;				//��Ÿ �÷���. ����/���ٽ� ���ۿ��� ������� �����Ƿ� 0����.
		} 	D3D11_TEXTURE2D_DESC;
		*/

		hr = this->m_Device->CreateTexture2D(
			&depthStencilDesc,							//������ �ؽ��ĸ� �����ϴ� ����ü
			NULL,										//�ؽ��Ŀ� ä�� �ʱ� �ڷ�. ����/���ٽ� ���ۿ����� ���� �ʿ����
			this->depthStencilBuffer.GetAddressOf());	//���� ���ٽ� ���۸� ����Ű�� �����͸� �����ش�.
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = this->m_Device->CreateDepthStencilView(
			this->depthStencilBuffer.Get(),			//�並 �����ϰ��� �ϴ� �ڿ�
			NULL,									//D3D11_DEPTH_STENCIL_VIEW_DESC ����ü�� ����Ű�� ������. �ڿ� �ڷ��� ����. ���� �ʿ� ����
			this->depthStencilView.GetAddressOf()); //���� ���ٽ� �並 �����ش�.
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		//******************************
		//�ĸ� ���ۿ� ���̽��ٽ� ���۸� ����������, ����� ���������� ��� ���ձ⿡ ����
		//�� ������ ���ľ� ��μ� �ڿ����� ������������ ���� ���� ����.���ٽ� ���۷� �ۿ�
		this->m_DeviceContext->OMSetRenderTargets(1, this->m_RenderTargetView.GetAddressOf(), this->depthStencilView.Get());
		//ù��° �Ķ���ʹ� ������ �ϴ� ���� ����� ���� ->���⿡���� �ϳ�.
		//�ι�° ����°��, �� �ڿ���.

		//Create depth stencil state ���ٽ� & ����
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->m_Device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//���ٽ� draw mask
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_drawMask(D3D11_DEFAULT);
		depthstencildesc_drawMask.DepthEnable = FALSE;
		depthstencildesc_drawMask.StencilEnable = TRUE;

		depthstencildesc_drawMask.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		depthstencildesc_drawMask.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

		depthstencildesc_drawMask.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		depthstencildesc_drawMask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_drawMask.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;

		hr = this->m_Device->CreateDepthStencilState(&depthstencildesc_drawMask, this->depthStencilState_drawMask.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state for Drawing Mask.");

		//���ٽ� apply mask
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_applyMask(D3D11_DEFAULT);
		depthstencildesc_applyMask.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		depthstencildesc_applyMask.StencilEnable = TRUE;

		depthstencildesc_applyMask.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		depthstencildesc_applyMask.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

		depthstencildesc_applyMask.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthstencildesc_applyMask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_applyMask.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;

		//hr = this->device->CreateDepthStencilState(&depthstencildesc_applyMask, this->depthStencilState_applyMask.GetAddressOf());
		//COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state for applying Mask.");

		//**********************
		//����Ʈ ����� & ����
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		this->m_DeviceContext->RSSetViewports(1, &viewport); //�з����� : ������ ���� ����Ʈ ����, ������ ����Ʈ ������,
		/* ����Ʈ ����ü
		typedef struct D3D11_VIEWPORT
		{
		FLOAT TopLeftX; //â�� ��ġ�� ũ��, ������ ����Ʈ�� ǥ���� ���� ���α׷� â�� Ŭ���̾�Ʈ ���� ���簢��
		FLOAT TopLeftY;
		FLOAT Width;
		FLOAT Height;
		FLOAT MinDepth; //�ּ� �ִ� ���� ���� ��. �����ϸ� �⺻������ ������ ��.
		FLOAT MaxDepth;
		} 	D3D11_VIEWPORT;
		*/

		//**********************
		//�����Ͷ����� state ����
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		//rasterizerDesc.CullMode = D3D11_CULL_NONE;
		hr = this->m_Device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf()); 
		//���� ����ü�� �����ؼ� ��� �����Ͷ����� ������Ʈ�� ��������. ���߿� �ʿ��� ������Ʈ�� RSSetState �Լ��� �����ϸ� �ȴ�.
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
		/*�����Ͷ����� desc
		typedef struct D3D11_RASTERIZER_DESC
		{
		D3D11_FILL_MODE FillMode;	//���̾� ������ �������� ��� D3D11_FILL_WIREFRAME, solid�������� ��� D3D11_FILL_SOLID
		D3D11_CULL_MODE CullMode;	//������ ��Ȱ��ȭ�Ϸ��� D3D11_CULL_NONE, �ĸ� �ﰢ���� �����ؼ� ���ܽ�Ű���� D3D11_CULL_BACK(FRONT�� ����) 
		BOOL FrontCounterClockwise;	//true�� �� ī�޶� ���� �ݽð�������� ���� �ﰢ���� �������� ����. �ð�����̸� false
		INT DepthBias;
		FLOAT DepthBiasClamp;
		FLOAT SlopeScaledDepthBias;
		BOOL DepthClipEnable;
		BOOL ScissorEnable;
		BOOL MultisampleEnable;
		BOOL AntialiasedLineEnable;
		} 	D3D11_RASTERIZER_DESC;
		*/


		//cull front �����Ͷ����� state ����
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->m_Device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//**********************
		//Create Blend state
		D3D11_BLEND_DESC blendDesc = { 0 };
		/*���� ���� ���� ����ü
		typedef struct D3D11_BLEND_DESC
		{
		BOOL AlphaToCoverageEnable;	//�⺻�� false, true�� �����ϸ� �Ĺ� �ٻ�� ���� �����ϱ⿡ ������ alpha-to-coverage ��ȯ�� Ȱ��ȭ
		BOOL IndependentBlendEnable;	//�⺻�� false, true �϶� �� ���� ���(������)���� ȥ���� ���������� ����.
		D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[ 8 ];	//�� �迭�� i��° ���Ҵ� i��° ���� ��� ������ ȥ�� ������ ���� ����ü
		} 	D3D11_BLEND_DESC;
		*/

		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		//blend alpha : Final alpha = source alpha * source alpha factor +(blend Operation) destination alpha * destination alpha factor

		rtbd.BlendEnable = true;									//ȥ�� Ȱ��ȭ ����
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;			//RGB���� ȥ���� ���� ȥ�� ���
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;	//RGB���� ȥ���� ��� ȥ�� ���
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;			//RGB���� ȥ�� ������
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;			//���� ���� ȥ���� ���� ȥ�� ���
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;		//���� ���� ȥ���� ��� ȥ�� ���
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;		//�˹� ���� ȥ�� ������
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;	//ȥ�� ����� � ���� ä�ο� ����� ������ ����

		blendDesc.RenderTarget[0] = rtbd;

		hr = this->m_Device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");
		/* //�����������̽� ���� �޼ҵ�
		virtual HRESULT STDMETHODCALLTYPE CreateBlendState( 
			_In_  const D3D11_BLEND_DESC *pBlendStateDesc, //ȥ�� ���¸� �����ϴ� ����ü
			_COM_Outptr_opt_  ID3D11BlendState **ppBlendState) = 0; //������ ���� �������̽��� �����ش�.
		*/

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->m_DeviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->m_Device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->m_Device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool GraphicsManager::InitializeShaders()
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

	Load_Shader_File(shaderfolder);

	//2D shader
	D3D11_INPUT_ELEMENT_DESC layout2D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
	};

	UINT numElements2D = ARRAYSIZE(layout2D);

	if (!vertexshader_2d.Initialize(this->m_Device, shaderfolder + L"vertexshader_2d.cso", layout2D, numElements2D, "vertexshader_2d"))
		return false;

	if (!pixelshader_2d.Initialize(this->m_Device, shaderfolder + L"pixelshader_2d.cso", "pixelshader_2d"))
		return false;

	if (!pixelshader_nolight.Initialize(this->m_Device, shaderfolder + L"pixelshader_nolight.cso", "pixelshader_nolight"))
		return false;

	if (!pixelshader_2d_discard.Initialize(this->m_Device, shaderfolder + L"pixelshader_2d_discard.cso", "pixelshader_2d_discard"))
		return false;
	try {
		//��� ���� �ʱ�ȭ
		HRESULT hr;
		hr = this->cb_vs_vertexshader.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize constant buffer.");

		hr = this->cb_vs_vertexshader_2d.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize 2d constant buffer.");

		hr = this->cb_vs_boneData.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize bone constant buffer.");

		hr = this->cb_ps_light.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize constant buffer.");

		this->cb_ps_light.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		this->cb_ps_light.data.ambientLightStrength = 1.0f;

		hr = this->cb_ps_fog.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize fog constant buffer.");

		hr = this->cb_ps_simplelight.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize simlight constant buffer.");

		hr = this->cb_ps_cameraPos.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize cam constant buffer.");

		hr = this->cb_ps_cameraPos.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize cam constant buffer.");

		hr = this->cb_ps_light_array.Initialize(this->m_Device.Get(), this->m_DeviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to Initialize light array constant buffer.");

		m_DeviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(1, 1, this->cb_ps_cameraPos.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(2, 1, this->cb_ps_simplelight.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(3, 1, this->cb_ps_fog.GetAddressOf());
		m_DeviceContext->PSSetConstantBuffers(5, 1, this->cb_ps_light_array.GetAddressOf());
	}
	catch (COMException exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool GraphicsManager::InitializeTextures()
{
	mTextureBuffer.emplace_back(this->m_Device.Get(), CustomColors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE);
	mTextureBuffer.back().Name = "Unhandled Texture";
	mTextureBuffer.back().ID = mTextureBuffer.size() - 1;
	mTextureMap.insert(std::make_pair("Unhandled Texture", mTextureBuffer.back().ID));

	mTextureBuffer.emplace_back(this->m_Device.Get(), CustomColors::Black, aiTextureType::aiTextureType_DIFFUSE);
	mTextureBuffer.back().Name = "Black Texture";
	mTextureBuffer.back().ID = mTextureBuffer.size() - 1;
	mTextureMap.insert(std::make_pair("Black Texture", mTextureBuffer.back().ID));

	mTextureBuffer.emplace_back(this->m_Device.Get(), CustomColors::White, aiTextureType::aiTextureType_DIFFUSE);
	mTextureBuffer.back().Name = "White Texture";
	mTextureBuffer.back().ID = mTextureBuffer.size() - 1;
	mTextureMap.insert(std::make_pair("White Texture", mTextureBuffer.back().ID));

	mTextureBuffer.emplace_back(this->m_Device.Get(), CustomColors::UnloadedTextureColor, aiTextureType::aiTextureType_DIFFUSE);
	mTextureBuffer.back().Name = "Unloaded Texture";
	mTextureBuffer.back().ID = mTextureBuffer.size() - 1;
	mTextureMap.insert(std::make_pair("Unloaded Texture", mTextureBuffer.back().ID));

	Load_Texture_File("Data\\Textures\\");
	return true;
}

void GraphicsManager::InitializeSimpleGeometry(ModelBuffer & _modelBuffer)
{
	Study_DX::Point geometryPoint;
	Study_DX::Box testbox;
	Study_DX::Box_Strange testbox2;
	Study_DX::Sphere testSphere(20, 5);
	Study_DX::Cylinder testCylinder;
	Study_DX::Plane testplane;

	Model *model;

	model = new Model();
	if (!model->Initialize(geometryPoint.vertices, geometryPoint.vertexSize, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return;
	}
	model->mName = geometryPoint.name;
	_modelBuffer.buffer.push_back(model);
	mModelMap.insert(std::make_pair(geometryPoint.name, model));


	model = new Model();
	if (!model->Initialize(testbox.vertices, testbox.vertexSize, testbox.indices, testbox.indexSize, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return;
	}
	model->mName = testbox.name;
	_modelBuffer.buffer.push_back(model);
	mModelMap.insert(std::make_pair(testbox.name, model));

	model = new Model();
	if (!model->Initialize(testbox2.vertices, testbox2.vertexSize, testbox2.indices, testbox2.indexSize, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return;
	}
	model->mName = testbox2.name;
	_modelBuffer.buffer.push_back(model);
	mModelMap.insert(std::make_pair(testbox2.name, model));

	model = new Model();
	if (!model->Initialize(&testSphere.vertices, &testSphere.indices, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return;
	}
	model->mName = testSphere.name;
	_modelBuffer.buffer.push_back(model);
	mModelMap.insert(std::make_pair(testSphere.name, model));

	model = new Model();
	if (!model->Initialize(&testCylinder.vertices, &testCylinder.indices, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return;
	}
	model->mName = testCylinder.name;
	_modelBuffer.buffer.push_back(model);
	mModelMap.insert(std::make_pair(testCylinder.name, model));

	model = new Model();
	if (!model->Initialize(testplane.vertices, testplane.vertexSize, testplane.indices, testplane.indexSize, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return;
	}
	model->mName = testplane.name;
	_modelBuffer.buffer.push_back(model);
	mModelMap.insert(std::make_pair(testplane.name, model));
}

bool GraphicsManager::InitializeScene()
{//���� ���߿� �̸� �����ؾߵ�
	try {
		//�ӽ� �׽�Ʈ��
		std::string cat_filepath = "Data\\Objects\\Test_cat\\12221_Cat_v1_l3.obj";
		std::string nanosuit_filepath = "Data\\Objects\\nanosuit\\nanosuit.obj";

		//�ؽ��� �ҷ�����
		//HRESULT hr;
		/*hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\test.png", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");*/

		//Initialize Constant buffer(s)


		//���� ������ �ʱ�ȭ
		if (!light.Initialize(mModelMap["light.fbx"])) {
			MessageBoxA(NULL, "Initialize light Error", "Error", MB_ICONERROR);
			return false;
		}

		/*for (std::vector<GameObject_v2*>::iterator it = all_gameObj->begin(); it != all_gameObj->end(); it++) {
			for (std::vector<Component*>::iterator it_compo = (*it)-> begin(); it != all_gameObj->end(); it++) {

			}
		}*/

		//��������Ʈ ����
		//if (!sprite.Initialize(this->device.Get(), this->deviceContext.Get(), 256, 256, "Data/Textures/circle.png", cb_vs_vertexshader_2d))
		//{
		//	MessageBoxA(NULL, "Initialize sprite Error", "Error", MB_ICONERROR);
		//	return false;
		//}
		//sprite.SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		sprite.SetPosition(DirectX::XMFLOAT3(windowWidth / 2 - sprite.GetWidth() / 2, windowHeight / 2 - sprite.GetHeight() / 2, 0.0f));

		Camera2D.SetProjectionValues((float)windowWidth, (float)windowHeight, 0.0f, 1.0f);

		Camera3D.SetPosition(0.0f, 0.0f, -2.0f);
		Camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

		/*for (std::vector<GameObject_v2*>::iterator it = all_gameObj->begin(); it != all_gameObj->end(); it++) {
			(*it)->renderer.Renderer_Initialize(this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader);
		}*/
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}



	return true;
}

bool GraphicsManager::InitializeTerrain(TerrainModelBuffer & _terrainmodelBuffer)
{
	for (auto it = m_Terrains.begin(); it != m_Terrains.end(); it++) {
		Model *model = new Model();
		TERRAIN_INIT_DESC desc = (*it)->TerrainProcess((*it)->heightFilePath);
		model->Initialize(&desc.vertexBuffer, &desc.indexBuffer, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer);
		_terrainmodelBuffer.buffer.push_back(model);
		(*it)->GameObject.renderer.SetModel(model);
		//(*it)->gameObject->mGameObjectName = "";
	}
	return true;
}

bool GraphicsManager::InitializeDebugDraw()
{
	m_states = std::make_unique<DirectX::CommonStates>(m_Device.Get());
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_DeviceContext.Get());

	m_effect = std::make_unique<DirectX::BasicEffect>(m_Device.Get());
	m_effect->SetVertexColorEnabled(true);


	{
		void const* shaderByteCode;
		size_t byteCodeLength;

		m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
		HRESULT hr = m_Device->CreateInputLayout(
			DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			debug_layout.ReleaseAndGetAddressOf());
		if (FAILED(hr)) assert("Debug intial fail." && 1 == 0);
	}

	return true;
}

void GraphicsManager::Load_Shader_File(std::wstring & _ExeFilePath) //���߿� ���̴� �ʱ�ȭ �Լ��� ��ĥ ��
{
	//���� �ȿ� hlsl���� Ȯ�� �� 
	//std::string path = "hlsl\\*.hlsl"; // hlsl ���� ���. ���� ������Ʈ ���ϰ� ���� ���� �ȿ� �ִٰ� ����.
	std::string pixelShader_path = "hlsl\\PixelShader\\*.hlsl";
	std::string vertexShader_path = "hlsl\\VertexShader\\*.hlsl";
	std::string geometryShader_path = "hlsl\\GeometryShader\\*.hlsl";
	std::string shader_filename;
	struct _finddata_t fd;
	intptr_t handle;

	//3D shader
	D3D11_INPUT_ELEMENT_DESC layout3D[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		//{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
	};
	/* �Է� ��ġ ��ü, ���� ������ direct3d���� ����
	typedef struct D3D11_INPUT_ELEMENT_DESC
	{
	LPCSTR SemanticName;				//���п� �ο��� ���ڿ� �̸�
	UINT SemanticIndex;					//semantic�� �ο��� ����, ���� �ϳ��� �ؽ��� ��ǥ�� �������� �� �������� ����
	DXGI_FORMAT Format;					//���� ������ �ڷ� ����
	UINT InputSlot;						//�� ������ �ڷᰡ ���޵� ���� ���� ������ ����
	UINT AlignedByteOffset;				//�Է� ������ �ϳ��� ����ϴ� ���. ���� ����ü�� ���� ��ġ�� �� ���� ������ ������ġ�� ������(����Ʈ ����)
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
	} 	D3D11_INPUT_ELEMENT_DESC;
	*/

	//skinned mesh vertex
	D3D11_INPUT_ELEMENT_DESC layout3D_skinned[] = {
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
		{"BONEID", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
		{"BONEWEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
	};

	UINT numElements3D = ARRAYSIZE(layout3D);
	UINT numElements_skinned = ARRAYSIZE(layout3D_skinned);

	//vertex shader �ʱ�ȭ
	if ((handle = _findfirst(vertexShader_path.c_str(), &fd)) != -1L) {
		do {
			shader_filename = StringHelper::EraseFileExtension(fd.name);

			std::shared_ptr<VertexShader> vs(new VertexShader);
			if (!vs->Initialize(
				this->m_Device,
				_ExeFilePath + StringHelper::StringToWide(shader_filename + ".cso"),
				layout3D_skinned,
				numElements_skinned,
				shader_filename))
			{
				MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
			}
			vert_Shader_Buffer.push_back(vs.get());
			m_VshaderBuffer.insert(std::make_pair(shader_filename, vs));
		} while (_findnext(handle, &fd) == 0);
	}

	//pixel shader �ʱ�ȭ
	if ((handle = _findfirst(pixelShader_path.c_str(), &fd)) != -1L) {
		do {
			shader_filename = StringHelper::EraseFileExtension(fd.name);

			PixelShader *ps = new PixelShader;
			if (!ps->Initialize(
				this->m_Device,
				_ExeFilePath + StringHelper::StringToWide(shader_filename + ".cso"),
				shader_filename))
			{
				MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
			}
				
			pixel_Shader_Buffer.push_back(ps);
			ps->Name = shader_filename;
			m_PshaderBuffer.insert(std::make_pair(shader_filename, ps));

		} while (_findnext(handle, &fd) == 0);
	}

	//geometry shader �ʱ�ȭ
	if ((handle = _findfirst(geometryShader_path.c_str(), &fd)) != -1L) {
		do {
			shader_filename = StringHelper::EraseFileExtension(fd.name);

			GeometryShader *gs = new GeometryShader;
			if (!gs->Initialize(this->m_Device, _ExeFilePath + StringHelper::StringToWide(shader_filename + ".cso"), shader_filename))
				MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
			Geo_Shader_Buffer.push_back(gs);
			gs->Name = shader_filename;
			m_GshaderBuffer.insert(std::make_pair(shader_filename, gs));

		} while (_findnext(handle, &fd) == 0);
	}

	_findclose(handle);
}

void GraphicsManager::Load_Texture_File(const std::string & _TextureFolderPath)
{
	struct _finddata_t fd;
	intptr_t handle;

	std::string Texture_FileExtension[3] = { "jpg", "png", "bmp" }; //png, jpg, bmp ���� �˻�
	std::string Texture_filePath = _TextureFolderPath + "*.*";

	int ExtensionArraySize = ARRAYSIZE(Texture_FileExtension);

	if ((handle = _findfirst(Texture_filePath.c_str(), &fd)) != -1L) {
		do {
			//�˻� ������ �����϶�, ���� ���丮 �˻�
			if (fd.attrib & _A_SUBDIR && (fd.name != std::string(".")) && (fd.name != std::string(".."))) {
				std::string childPath = _TextureFolderPath + fd.name + "\\";
				Load_Texture_File(childPath);
				continue;
			}

			std::string fileExtension = StringHelper::GetFileExtension(fd.name);
			bool flag = false;

			//Ȯ���� ��ġ ���� �˻�
			for (int Index = 0; Index < ExtensionArraySize; Index++) {
				if (fileExtension == Texture_FileExtension[Index]) {
					flag = true;
					break;
				}
			}
			//��ġ�ϴ� Ȯ���� ������ ���� ���� �˻�
			if (!flag) continue;

			std::string Texture_filename = _TextureFolderPath + fd.name;
			mTextureBuffer.emplace_back(m_Device.Get(), Texture_filename, aiTextureType::aiTextureType_DIFFUSE);
			mTextureBuffer.back().Name = fd.name;
			mTextureBuffer.back().ID = mTextureBuffer.size() - 1;
			mTextureMap.insert(std::make_pair(fd.name, mTextureBuffer.back().ID));

		} while (_findnext(handle, &fd) == 0);
	}
	_findclose(handle);
}

void GraphicsManager::InitializeModel(ModelBuffer & modelBuffer)
{
	std::string filePath = "Data\\Objects\\";
	InitializeModel(modelBuffer, filePath);
}

void GraphicsManager::InitializeModel(ModelBuffer & modelBuffer, std::string & filePath)
{
	std::string path = filePath + "*.*";
	static std::string debug_string = "";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) != -1L) {
		do {
			if (StringHelper::GetFileExtension(fd.name) == "fbx") {
				debug_string += (std::string)fd.name + ", ";

				Model *model = new Model();
				if (!model->Initialize(filePath + fd.name, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, this->cb_vs_boneData, mTextureMap, mTextureBuffer)) {
					MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
					return;
				}
				model->mName = fd.name;
				modelBuffer.buffer.push_back(model);
				mModelMap.insert(std::make_pair(fd.name, model));
			}
			else if (StringHelper::GetFileExtension(fd.name) == "obj") {
				debug_string += (std::string)fd.name + ", ";

				Model *model = new Model();
				if (!model->Initialize(filePath + fd.name, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, this->cb_vs_boneData, mTextureMap, mTextureBuffer)) {
					MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
					return;
				}
				model->mName = fd.name;
				modelBuffer.buffer.push_back(model);
				mModelMap.insert(std::make_pair(fd.name, model));
			}
			else if (fd.attrib & _A_SUBDIR && (fd.name != std::string(".")) && (fd.name != std::string(".."))) {
				std::string childPath = filePath + fd.name + "\\";
				InitializeModel(modelBuffer, childPath);
			}

		} while (_findnext(handle, &fd) == 0);
	}
	_findclose(handle);
}

bool GraphicsManager::Initialize_MoreRenderTarget()
{
	//����Ÿ�� �߰��ϱ�
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	try {
		///////////////////////// Map's Texture
	// Initialize the  texture description.
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Setup the texture description.
		// We will have our map be a square
		// We will need to have this texture bound as a render target AND a shader resource
		textureDesc.Width = windowWidth;
		textureDesc.Height = windowHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Create the texture
		m_Device->CreateTexture2D(&textureDesc, NULL, AUX_rt_Texture.GetAddressOf());

		/////////////////////// Map's Render Target
		// Setup the description of the render target view.
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		m_Device->CreateRenderTargetView(AUX_rt_Texture.Get(), &renderTargetViewDesc, AUX_rtv.GetAddressOf());

		/////////////////////// Map's Shader Resource View
		// Setup the description of the shader resource view.
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Create the shader resource view.
		m_Device->CreateShaderResourceView(AUX_rt_Texture.Get(), &shaderResourceViewDesc, AUX_srv.GetAddressOf());
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool GraphicsManager::Initialize_Skybox()
{
	std::string filename[6] = { //������ ���߿�
		"Data\\Skybox\\Test1\\oasisnight_ft_p.png",	// +X
		"Data\\Skybox\\Test1\\oasisnight_bk_p.png",	// -X
		"Data\\Skybox\\Test1\\oasisnight_up_p.png",	// +Y
		"Data\\Skybox\\Test1\\oasisnight_dn_p.png",	// -Y
		"Data\\Skybox\\Test1\\oasisnight_rt_p.png",	// +Z
		"Data\\Skybox\\Test1\\oasisnight_lf_p.png"	// -Z
	};

	Study_DX::Sphere testSphere(30, 30);
	Model * model = new Model();
	if (!model->Initialize(&testSphere.vertices, &testSphere.indices, this->m_Device.Get(), this->m_DeviceContext.Get(), this->cb_vs_vertexshader, mTextureMap, mTextureBuffer)) {
		MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
		return false;
	}

	if (!mSkybox.Initialize(filename, m_Device.Get(), m_DeviceContext.Get(), model)) {
		MessageBoxA(NULL, "Skybox Initialize error.", ERROR, MB_ICONERROR);
		return false;
	}
	return true;
}

#pragma endregion

#pragma region Method - Update

void GraphicsManager::RenderFrame()
{
	//��� ���� ����
	cb_ps_light.data.dynamicLightColor = light.lightColor;
	cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;
	cb_ps_light.ApplyChanges();
	SetConstantBuffer();


	m_DeviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(1, 1, this->cb_ps_cameraPos.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(2, 1, this->cb_ps_simplelight.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(3, 1, this->cb_ps_fog.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(5, 1, this->cb_ps_light_array.GetAddressOf());
	//deviceContext->GSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());
	m_DeviceContext->GSSetConstantBuffers(1, 1, this->cb_ps_cameraPos.GetAddressOf());

	//this->deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(this->m_RenderTargetView.Get(), bgcolor); //ȭ�� �����ϰ� �����
	this->m_DeviceContext->OMSetRenderTargets(1, AUX_rtv.GetAddressOf(), depthStencilView.Get());
	this->m_DeviceContext->ClearRenderTargetView(AUX_rtv.Get(), bgcolor);
	m_DeviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);




	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //���� �׸��� ������ ����. ->�ﰢ������ �׸��ڴ�.
	m_DeviceContext->RSSetState(this->rasterizerState.Get());//�����Ͷ����� ���� ����
	m_DeviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(blendState.Get(), blendFactors, 0xFFFFFFFF);
	/* OM �ܰ迡 ���� ������Ʈ ����
	virtual void STDMETHODCALLTYPE OMSetBlendState(
		_In_opt_  ID3D11BlendState *pBlendState,	//��ġ�� ������ ȥ�� ���� ��ü
		_In_opt_  const FLOAT BlendFactor[4],		//�ε��Ҽ��� �� �� ���� �迭�� ����Ű�� ������, RGBA���� ���� ����
		_In_  UINT SampleMask) = 0;					//����ǥ��ȭ�� ���ϴ� �ִ� 32���� ǥ��. ������ 0xFFFFFFFF ���
	*/
	m_DeviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());



	{
		//��� ������Ʈ �� ������
		const DirectX::XMMATRIX viewProjectionMatrix = Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix();

		for (std::vector<std::shared_ptr<GameObject_v2>>::iterator it = gameObjBuffer->begin(); it != gameObjBuffer->end(); it++) {
			bool Render_enable = (*it)->renderer.enabled;
			bool GameObject_enable = (*it)->enabled;
			if (GameObject_enable && Render_enable) {
				(*it)->renderer.Draw(viewProjectionMatrix);
			}
		}
	}

	//����� �����
	//DrawGrid();
	//DebugDrawTest();

	{
		m_DeviceContext->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		light.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
	}
#pragma region CheckFrame_
	//Draw text + ������ ����
	static int fpsCounter = 0;
	static float fpsTime = 1.0f;
	static float deltaTime;
	static std::string fpsString = "FPS: None \ndeltaTime : None";

	//deltaTime = Time->GetDeltaTime();
	fpsCounter += 1;
	fpsTime -= deltaTime;

	if (fpsTime < 0.0f) {
		fpsString = "FPS: " + std::to_string(fpsCounter) + "\ndeltaTime : " + std::to_string(deltaTime);
		fpsCounter = 0;
		fpsTime = 1.0f;
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();


#pragma endregion


}


void GraphicsManager::RenderCollider_v2Debug(std::vector<std::shared_ptr<Collider_v2>>* physicsCompoBuffer)
{
	this->m_DeviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	this->m_DeviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	this->m_DeviceContext->RSSetState(m_states->CullNone());

	m_effect->DirectX::BasicEffect::SetView(Camera3D.GetViewMatrix());
	m_effect->SetProjection(Camera3D.GetProjectionMatrix());

	m_effect->Apply(m_DeviceContext.Get());

	this->m_DeviceContext->IASetInputLayout(debug_layout.Get());

	m_batch->Begin();

	//auto& pm = Module::GetPhysicsModule().Get;
	//for (auto it = physicsCompoBuffer->begin(); it != physicsCompoBuffer->end(); it++) {
	//	bool Component_valid = (*it)->Enabled;
	//	bool GameObject_valid = (*it)->GameObject.enabled;

	//	if (Component_valid && GameObject_valid == false) continue;



	//	COLLIDER_DEBUG_MODEL_VER2 desc = (*it)->Get_DebugModelType();
	//	switch (desc.typeNum) {
	//	case 0:
	//		//Draw(m_batch.get(), *desc.aabbPtr, DirectX::Colors::White);
	//		break;
	//	case 1:
	//		Draw(m_batch.get(), desc.mDeubgBox, DirectX::Colors::White);
	//		break;
	//	case 2:
	//		Draw(m_batch.get(), desc.mDebugSphere, DirectX::Colors::White);
	//		break;
	//	case 3:
	//		Draw(m_batch.get(), desc.mDebugSphere, DirectX::Colors::White);
	//		desc.mDebugSphere.Center.x = desc.auxVal.x;
	//		desc.mDebugSphere.Center.y = desc.auxVal.y;
	//		desc.mDebugSphere.Center.z = desc.auxVal.z;
	//		Draw(m_batch.get(), desc.mDebugSphere, DirectX::Colors::White);
	//		desc.mDebugSphere.Center.x = desc.auxVal2.x;
	//		desc.mDebugSphere.Center.y = desc.auxVal2.y;
	//		desc.mDebugSphere.Center.z = desc.auxVal2.z;
	//		desc.mDebugSphere.Radius = 0.1f;
	//		Draw(m_batch.get(), desc.mDebugSphere, DirectX::Colors::Yellow);
	//		break;
	//	}
	//}

	m_batch->End();
	this->m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), depthStencilView.Get());

}

bool GraphicsManager::SetConstantBuffer()
{
	cb_ps_cameraPos.data.dynamicCameraPosition = Camera3D.GetPositionFloat3();
	cb_ps_cameraPos.ApplyChanges();

	float * fogCptr = fogC.Get_ptr();
	cb_ps_fog.data.fogColor = DirectX::XMFLOAT3(fogCptr[0], fogCptr[1], fogCptr[2]);
	cb_ps_fog.data.fogEnabled = fogEnabled;
	cb_ps_fog.data.fogRange = fogRange;
	cb_ps_fog.data.fogStart = fogStart;
	cb_ps_fog.ApplyChanges();

	cb_ps_simplelight.data.ambientLightColor = cb_ps_light.data.ambientLightColor;
	cb_ps_simplelight.data.ambientLightStrength = cb_ps_light.data.ambientLightStrength;
	cb_ps_simplelight.ApplyChanges();

	//cb_ps_array.data.
	if (m_Lights.size() != 0) {
		LIGHT_INFO_DESC lightDesc = m_Lights[0]->GetInfoDesc();
		cb_ps_light_array.data.directional_light[0].lightColor = *lightDesc.lightColor;
		cb_ps_light_array.data.directional_light[0].lightStrength = *lightDesc.lightStrength;
		cb_ps_light_array.data.directional_light[0].lightVec = *lightDesc.lightVec;
		cb_ps_light_array.ApplyChanges();
	}


	return true;
}

void GraphicsManager::DrawGrid()
{

}

void GraphicsManager::DrawSkyBox()
{

	const DirectX::XMMATRIX vpMatrix = Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix();
	const DirectX::XMMATRIX wMatrix = DirectX::XMMatrixTranslationFromVector(Camera3D.GetPositionVector());

	m_DeviceContext->IASetInputLayout(this->vert_Shader_Buffer[5]->GetInputLayout()); //IA�� �Է��� ��ġ ����
	m_DeviceContext->VSSetShader(this->vert_Shader_Buffer[5]->GetShader(), NULL, 0); //�׸� �� �� ���̴� ����
	m_DeviceContext->PSSetShader(this->pixel_Shader_Buffer[6]->GetShader(), NULL, 0); //�׸� �� �� ���̴� ����
	m_DeviceContext->GSSetShader(NULL, NULL, 0);

	mSkybox.Draw(wMatrix, vpMatrix);
}

void GraphicsManager::ProcessUI()
{
	//Start the Dear ImGui Frame

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	UIspace::EditorUI(AUX_srv.Get());
	//Create ImGui Test Window
	//UIspace::EditorUI();


	ImGui::Begin("Light Controls");
	if (ImGui::IsItemClicked()) {
		ImGui::DragFloat3("Ambient Light Color", &this->cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Ambient Light Strenght", &this->cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	}

	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &this->light.lightColor.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &this->light.lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &this->light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &this->light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &this->light.attenuation_c, 0.01f, 0.0f, 10.0f);
	if (ImGui::Button("Assert Button", ImVec2(325, 25))) assert(1 == 0);

	ImGui::End();

	ImGui::Begin("Fog Control Test");
	ImGui::DragFloat3("fog Color", fogC.Get_ptr(), 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("fog Start", &fogStart, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("fog Range", &fogRange, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("fog Start2", &fogStart, 0.1f, 0.0f, 100.0f);
	ImGui::DragInt("Fog Enable", &fogEnabled, 1.0f, 0, 1);
	ImGui::End();

	GameObject_v2 * gameObjPick = gameObjBuffer->at(0).get();
	ImGui::Begin("GameObject Transform Control");
	//ImGui::DragFloat3("transform", &gameObjPick->transform..x, 0.1f, -500.0f, 500.0f);
	ImGui::DragFloat3("scale", &gameObjPick->transform.scale.x, 0.01f, 0.0f, 10.0f);
	if (ImGui::Button("ScaleChange")) gameObjPick->transform.SetScale(0.7f, 0.7f, 0.7f);
	if (ImGui::Button("ScaleChange2")) gameObjPick->transform.SetScale(1.0f, 1.0f, 1.0f);
	if (ImGui::Button("Add BoxCollider")) gameObjPick->AddComponent<Animator>();
	if (ImGui::Button("Remove BoxCollider")) gameObjPick->RemoveComponent<Animator>();
	if (ImGui::Button("Anim play")) gameObjPick->GetComponent<Animator>()->Play();
	if (ImGui::Button("Anim stop")) gameObjPick->GetComponent<Animator>()->Stop();
	ImGui::End();


	if (mTextureBuffer.size() >= 1) {
		ImGui::Begin("Texture load Test");
		static int textureindex = 0;
		ImGui::DragInt("texture index", &textureindex, 1.0f, 0, (int)mTextureBuffer.size() - 1);
		ImVec2 scene_size = ImVec2(128.0f, 96.0f);
		if (textureindex < mTextureBuffer.size()) {
			ImGui::Image(mTextureBuffer[textureindex].GetTextureResourceView(), scene_size);
			ImGui::Text(mTextureBuffer[textureindex].Name.c_str());
		}

		ImGui::End();
	}

	if (mModelMap.size() >= 1) {
		ImGui::Begin("Model load Test");
		for (auto iter : mModelMap) {
			ImGui::Text(iter.first.c_str());
		}
		ImGui::End();
	}

	ImGui::Begin("Shader load check");
	ImGui::BeginChild("VShader", ImVec2(250, 150));
	for (auto iter : m_VshaderBuffer) {
		ImGui::Text(iter.first.c_str());
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("PShader", ImVec2(250, 150));
	for (auto iter : m_PshaderBuffer) {
		ImGui::Text(iter.first.c_str());
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("GShader", ImVec2(250, 150));
	for (auto iter : m_GshaderBuffer) {
		ImGui::Text(iter.first.c_str());
	}
	ImGui::EndChild();
	ImGui::End();


	//ImGui::Begin("Quaternion to Euler Test");
	//ImGui::DragFloat3("Base Euler rotation", &baseEuler.x, 0.1f, -360.0f, 360.0f);
	//midQuaternion = EulerToQuat(baseEuler.x, baseEuler.y, baseEuler.z);
	////DirectX::MQua
	//DirectX::XMStoreFloat4(&DxQuaternion, DirectX::XMQuaternionRotationRollPitchYaw(baseEuler.x * Deg2Rad, baseEuler.y * Deg2Rad, baseEuler.z * Deg2Rad));
	//ImGui::DragFloat4("Euler to Quaternion", &midQuaternion.x, 0.1f, -360.0f, 360.0f);
	//ImGui::DragFloat4("DX Quaternion", &DxQuaternion.x, 0.1f, -360.0f, 360.0f);
	//
	//finalEuler = quatToEuler3(midQuaternion.x, midQuaternion.y, midQuaternion.z, midQuaternion.w);
	//ImGui::DragFloat3("Quternion to Euler", &finalEuler.x, 0.1f, -360.0f, 360.0f);
	//finalEuler2 = quatToEuler3(DxQuaternion.x, DxQuaternion.y, DxQuaternion.z, DxQuaternion.w);
	//ImGui::DragFloat3("DX Quternion to Euler", &finalEuler2.x, 0.1f, -360.0f, 360.0f);
	//ImGui::End();



	/*int simple = gameObjBuffer->at(1)->GetComponent<BoxCollider>()->collidObj.size();
	int simple2 = gameObjBuffer->at(2)->GetComponent<BoxCollider>()->collidObj.size();
	ImGui::Begin("collide debug Test");
	ImGui::DragFloat3("pos", &gameObjBuffer->at(2)->GetComponent<BoxCollider>()->localCenter.x, 0.1f, -90.0f, 90.0f);
	ImGui::DragFloat3("Rotation", &gameObjBuffer->at(2)->GetComponent<BoxCollider>()->localRotation.x, 0.1f, -90.0f, 90.0f);
	ImGui::DragFloat3("scale", &gameObjBuffer->at(2)->GetComponent<BoxCollider>()->localScale .x, 0.1f, -90.0f, 90.0f);
	ImGui::DragFloat("speed", &gameObjBuffer->at(2)->GetComponent<cstest>()->speed, 0.1f, -90.0f, 90.0f);
	ImGui::DragInt("Collision1", &simple, 1, 0, 10);
	ImGui::DragInt("Collision2", &simple2, 1, 0, 10);
	ImGui::End();*/

	//ImGui::Begin("array Test");
	//ImGui::DragFloat3("DirecLightColor", &(*lightBuffer).at(0)->lightColor.x, 0.01f, 0.0f, 1.0f);
	//static DirectX::XMFLOAT3 test;
	//ImGui::DragFloat3("DirecLightVec", &test.x, 0.1f, -90.0f, 90.0f);
	//(*lightBuffer).at(0)->gameObject->transform.SetRotation(test);
	//ImGui::DragFloat("DirecLightstr", &(*lightBuffer).at(0)->lightStrength, 0.1f, 0.0f, 100.0f);
	////ImGui::DragFloat4("array Color0", &cb_ps_array.data.array_Color[0].x, 0.01f, 0.0f, 1.0f);
	////ImGui::DragFloat4("array Color1", &cb_ps_array.data.array_Color[1].x, 0.01f, 0.0f, 1.0f);
	////ImGui::DragFloat4("array Color2", &cb_ps_array.data.array_Color[2].x, 0.01f, 0.0f, 1.0f);
	////ImGui::DragFloat4("array Color3", &cb_ps_array.data.array_Color[3].x, 0.01f, 0.0f, 1.0f);
	////ImGui::DragInt("index", &cb_ps_array.data.index, 1.0f, 0, 3);
	//ImGui::End();

	/*ImGui::Begin("OBB Control Test");
	ImGui::DragFloat3("Obb Center", &obbBox.Center.x, 0.1f, 0.0f, 50.0f);
	ImGui::DragFloat3("Obb Extents", &obbBox.Extents.x, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat4("Obb Orientation", &obbBox.Orientation.x, 0.1f, 0.0f, 100.0f);
	ImGui::End();*/

	/*ImGui::Begin("memory Test");
	static MemoryBOOM * tmp_mb = gameObjBuffer->at(0)->GetComponent<MemoryBOOM>();
	ImGui::DragInt("Obb Center", &tmp_mb->boomNum, 0, 10000, 10000000);
	if (ImGui::Button("BOOM", ImVec2(325, 25))) tmp_mb->BOOM();
	if (ImGui::Button("Delete", ImVec2(325, 25))) tmp_mb->Delete();
	ImGui::End();*/

	/*ImGui::Begin("Scale Control Test");
	ImGui::DragFloat3("Scale", &gameObjBuffer->at(2)->transform.scale.x, 0.1f, 0.0f, 50.0f);
	ImGui::End();*/

	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsManager::DebugDrawTest()
{
	this->m_DeviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	this->m_DeviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	this->m_DeviceContext->RSSetState(m_states->CullNone());

	m_effect->SetView(Camera3D.GetViewMatrix());
	m_effect->SetProjection(Camera3D.GetProjectionMatrix());

	m_effect->Apply(m_DeviceContext.Get());

	this->m_DeviceContext->IASetInputLayout(debug_layout.Get());

	m_batch->Begin();
	Draw(m_batch.get(), boundingFrustum, DirectX::Colors::White); // BoundingFrustum
	//Draw(m_batch.get(), boundingBox, Colors::Blue); // BoundingBox
	Draw(m_batch.get(), obbBox, DirectX::Colors::White); // BoundingOrientedBox
	//Draw(m_batch.get(), boundingSphere, Colors::Blue); // BoundingSphere

	m_batch->End();
}

void GraphicsManager::SwapBuffer()
{
	this->m_SwapChain->Present(1, NULL); //vsync ����, 1�̸� ������ ����ȭ, 0�̸� ������ �ִ�
}

ID3D11Device & GraphicsManager::GetDevice()
{
	return *(m_Device.Get());
}

ID3D11DeviceContext & GraphicsManager::GetDeviceContext()
{
	return *(m_DeviceContext.Get());
}

#pragma endregion

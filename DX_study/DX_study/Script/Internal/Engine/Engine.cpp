#include "Engine.h"
#include <functional>

#include "Timer.h"
#include "DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../Core/GameObject.h"
#include "../core/ObjectPool_Front.h"
#include "../Graphics/Graphics.h"
#include "../../Component/Animator.h"
#include "../../Component/Transform.h"
#include "../../Component/Behaviour.h"
#include "../../Component/Light.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"

using DirectX::operator*;
using namespace DirectX;

Engine* Engine::s_Ptr = nullptr;
const float Engine::s_FixedFrameRate = 0.05f;

Engine & Engine::Get()
{
	return *s_Ptr;
}

Engine::Engine() :
	m_Timer(std::make_unique<Timer>()),
	m_CurrentScene(std::make_unique<Scene>()),
	m_Graphics(std::make_unique<Graphics>())
{
	s_Ptr = this;
}

Engine::~Engine()
{
	s_Ptr = nullptr;
}

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	Core::LightPool::GetInstance();
	m_Timer->Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height)) {
		return false;
	}

	if (!m_Graphics->Initialize(this->render_window.GetHWND(), width, height)) {
		return false;
	}

	m_CurrentScene->Initialize();

	m_Graphics->GetDeviceResources().InitializeDebugLayout(m_CurrentScene->GetMainCam()->GetViewMatrix(), m_CurrentScene->GetMainCam()->GetProjectionMatrix());

	

	return true;
}

bool Engine::ProcessMessage() {
	return this->render_window.ProcessMessage();
}

void UpdateBehaviour(const std::shared_ptr<Behaviour> & behaviour) {
	if (behaviour->Enable) {
		behaviour->Update();
	}
}

void Engine::Update() {
	m_Timer->Tick();
	keyboard.Update();
	mouse.Update();
	Core::Pool<Behaviour>::GetInstance().ForEach(UpdateBehaviour);
	m_CurrentScene->Update();
}

void Engine::FixedUpdate()
{
	Core::Pool<Animator>::GetInstance().ForEach(UpdateBehaviour);
}

void Engine::RenderFrame()
{
	static auto drawFunc = std::bind(&Graphics::PushToRenderQueue, m_Graphics.get(), std::placeholders::_1);
	auto& dr = m_Graphics->GetDeviceResources();

	m_Graphics->RenderFrame();

	//shadowmap
	m_Graphics->DrawShadowMap(Core::Find<Light>("Light")->GetComponent<SpotLight>());

	m_Graphics->SetRenderTarget(m_Graphics->GetDeviceResources().GetRTVaddress(0), DeviceResources::DeferredRenderChannelCount);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);
	m_Graphics->DrawSkybox();

	m_Graphics->SetRenderTarget(dr.GetRTVaddress(DeviceResources::DeferredRenderChannelCount));
	m_Graphics->PostProcess();
	m_Graphics->DrawGuiDebug();
	m_Graphics->SetRenderTarget(dr.GetBaseRTVaddress());
	m_Graphics->DrawGui();
	m_Graphics->SwapBuffer();
}
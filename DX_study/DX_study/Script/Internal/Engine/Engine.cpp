#include "Engine.h"
#include <functional>

#include "Timer.h"
#include "DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../Core/GameObject.h"
#include "../Graphics/Graphics.h"
#include "../../Component/Animator.h"
#include "../../Component/Transform.h"
#include "../../Component/Behaviour.h"
#include "../../Component/Light.h"
#include "../../GameObject/Camera.h"

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
	static auto drawFunc = std::bind(&Graphics::Render, m_Graphics.get(), std::placeholders::_1);
	auto& dr = m_Graphics->GetDeviceResources();

	m_Graphics->RenderBegin();

	m_Graphics->DrawShadowMap(Core::Find<GameObject>("Light")->GetComponent<SpotLight>());
	m_Graphics->RenderModels();
	m_Graphics->DrawSkybox();

	m_Graphics->ComputeShdaderTest();
	m_Graphics->PostProcess();
	//m_Graphics->DrawGuiDebug();
	m_Graphics->DrawGui();
	m_Graphics->RenderEnd();
}

void Engine::Run()
{
	float fixedTimeStamp = 0.0f;

	while (ProcessMessage()) {

		fixedTimeStamp += m_Timer->GetDeltaTime();
		if (fixedTimeStamp > Engine::s_FixedFrameRate) {
			FixedUpdate();
			fixedTimeStamp -= Engine::s_FixedFrameRate;
		}

		Update();
		RenderFrame();
	}
}

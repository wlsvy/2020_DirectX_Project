#include "Engine.h"
#include <functional>

#include "Timer.h"
#include "Profiler.h"
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

	Core::Pool<Object>::GetInstance().Clear();
}

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, UINT width, UINT height)
{
	m_Timer->Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height)) {
		return false;
	}

	if (!m_Graphics->Initialize(this->render_window.GetHWND(), width, height)) {
		return false;
	}

	m_CurrentScene->Initialize();
	Profiler::GetInstance().Initialize();

	m_Graphics->InitializeDebugLayout(m_CurrentScene->GetMainCam()->GetViewMatrix(), m_CurrentScene->GetMainCam()->GetProjectionMatrix());

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
	ScopedProfilingSample("Engine Update");
	m_Timer->Tick();
	
	UpdateInput();
	
	Core::Pool<Behaviour>::GetInstance().ForEach(UpdateBehaviour);

	m_CurrentScene->Update();

	Profiler::GetInstance().Update();
}

void Engine::UpdateInput()
{
	while (!keyboard.KeyBufferIsEmpty()) {
		auto key = keyboard.ReadKey();
		if (key.GetKeyCode() == VK_ESCAPE &&
			render_window.GetHWND() != NULL)
		{
			CloseWindow();
		}
	}
	while (!keyboard.CharBufferIsEmpty()) {
		keyboard.ReadChar();
	}
	mouse.Update();
}

void Engine::FixedUpdate()
{
	{
		ScopedProfilingSample("Animation Update");
		Core::Pool<Animator>::GetInstance().ForEach(UpdateBehaviour);
	}
}

void Engine::RenderFrame()
{
	ScopedProfilingSample("Render Frame");

	m_Graphics->RenderBegin();
	m_Graphics->Pass_ShadowMap(Core::Find<GameObject>("Light")->GetComponent<SpotLight>());
	m_Graphics->Pass_GBuffer();
	m_Graphics->Pass_SSAO();
	m_Graphics->Pass_Composition();	
	m_Graphics->Pass_PostProcess();
	//m_Graphics->Pass_Gizmo();
	m_Graphics->Pass_EditorUI();
	m_Graphics->RenderEnd();
}

void Engine::Run()
{
	float fixedTimeStamp = 0.0f;

	while (ProcessMessage()) {
		ScopedProfilingSample("Frame Updaet");
		fixedTimeStamp += m_Timer->GetDeltaTime();

		if (fixedTimeStamp > Engine::s_FixedFrameRate) 
		{
			FixedUpdate();

			fixedTimeStamp -= Engine::s_FixedFrameRate;
		}

		Update();
		RenderFrame();

		Profiler::GetInstance().Clear();
	}
}

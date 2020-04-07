#include "Engine.h"
#include <functional>

#include "Timer.h"
#include "DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../Core/GameObject.h"
#include "../Graphics/Graphics.h"
#include "../../Component/Transform.h"
#include "../../Component/Behaviour.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"

using DirectX::operator*;
using namespace DirectX;

Engine* Engine::s_Ptr = nullptr;

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
	Pool::ObjectPool<Behaviour>::GetInstance().ForEach(UpdateBehaviour);

	this->m_Graphics->gameObject->GetTransform().rotate(0.0f, 100 * m_Timer->GetDeltaTime(), 0.0f);

	m_CurrentScene->Update();
	this->m_Graphics->mainCam->UpdateViewMatrix();
}

void Engine::RenderFrame()
{
	static auto drawFunc = std::bind(&Graphics::Draw, m_Graphics.get(), std::placeholders::_1);

	m_Graphics->RenderFrame();
	m_Graphics->SetOmRenderTargetToAux();
	Pool::ObjectPool<Renderable>::GetInstance().ForEach(drawFunc);
	m_Graphics->SetOmRenderTargetToBase();
	m_Graphics->DrawImGui();
	m_Graphics->SwapBuffer();
}
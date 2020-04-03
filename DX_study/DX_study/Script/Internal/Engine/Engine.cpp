#include "Engine.h"
#include <functional>

#include "Timer.h"
#include "../Graphics/Graphics.h"
#include "DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../../Component/Transform.h"
#include "../../GameObject/GameObject.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"

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
	
	GameObject* ptr;
	{
		auto var = Pool::CreateInstance<GameObject>();
		ptr = var.get();
	}
	Pool::Destroy(ptr);

	return true;
}

bool Engine::ProcessMessage() {
	//if(keyboard.KeyIsPressed('P'))
	return this->render_window.ProcessMessage();
}

void Engine::Update() {
	m_Timer->Tick();
	float dt = m_Timer->GetDeltaTime();

	while (!keyboard.CharBufferIsEmpty()) {
		unsigned char ch = keyboard.ReadChar();
	} 
	while (!keyboard.KeyBufferIsEmpty()) {
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while (!mouse.EventBufferIsEmpty()) {
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown()) {//마우스 오른쪽 버튼 눌렀을 때 회전
			if (me.GetType() == MouseEvent::EventType::Raw_MOVE) {
				m_Graphics->mainCam->GetTransform().rotate((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0.0f);
			}
		}
	}

	this->m_Graphics->gameObject->GetTransform().rotate(0.0f, 100 * dt, 0.0f);

	//1인칭 형식 카메라 이동 조작
	float Camera3DSpeed = 6.0f;

	if (keyboard.KeyIsPressed(VK_SHIFT)) {
		Camera3DSpeed = 30.0f;
	}

	if (keyboard.KeyIsPressed('W')) {
		this->m_Graphics->mainCam->GetTransform().translate(this->m_Graphics->mainCam->GetTransform().GetForwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S')) {
		this->m_Graphics->mainCam->GetTransform().translate(this->m_Graphics->mainCam->GetTransform().GetBackwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A')) {
		this->m_Graphics->mainCam->GetTransform().translate(this->m_Graphics->mainCam->GetTransform().GetLeftVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D')) {
		this->m_Graphics->mainCam->GetTransform().translate(this->m_Graphics->mainCam->GetTransform().GetRightVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_SPACE)) {
		this->m_Graphics->mainCam->GetTransform().translate(0.0f, Camera3DSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z')) {
		this->m_Graphics->mainCam->GetTransform().translate(0.0f, -Camera3DSpeed * dt, 0.0f);
	}

	if (keyboard.KeyIsPressed('C')) {
		XMVECTOR lightPosition = this->m_Graphics->mainCam->GetTransform().GetPositionVector();
		lightPosition += this->m_Graphics->mainCam->GetTransform().GetForwardVector();
		m_Graphics->light->GetTransform().SetPosition(lightPosition);
		m_Graphics->light->GetTransform().SetRotation(this->m_Graphics->mainCam->GetTransform().GetRotationFloat3());
	}
	this->m_Graphics->mainCam->UpdateViewMatrix();
	m_CurrentScene->UpdateTransform();
}

void Engine::RenderFrame()
{
	static auto drawFunc = std::bind(&Graphics::Draw, m_Graphics.get(), std::placeholders::_1);

	m_Graphics->RenderFrame();
	Pool::ObjectPool<Renderable>::GetInstance().ForEach(drawFunc);
	m_Graphics->DrawImGui();
	m_Graphics->DrawFrameString();
	m_Graphics->SwapBuffer();
}

Graphics & Engine::GetGraphics()
{
	return *m_Graphics.get();
}

Timer & Engine::GetTimer()
{
	return *m_Timer.get();
}

Scene & Engine::GetCurrentScene()
{
	return *m_CurrentScene.get();
}
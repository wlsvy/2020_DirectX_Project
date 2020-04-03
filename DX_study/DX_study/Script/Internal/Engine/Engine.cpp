#include "Engine.h"

#include "Timer.h"
#include "../Graphics/Graphics.h"
#include "DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/Scene.h"
#include "../../Component/Transform.h"

Engine* Engine::s_Ptr = nullptr;

Engine & Engine::Get()
{
	return *s_Ptr;
}

Engine::Engine() :
	m_Timer(std::make_unique<Timer>()),
	m_CurrentScene(std::make_unique<Scene>())
{
	
	s_Ptr = this;
	auto light = Pool::CreateInstance<Light>();
}

Engine::~Engine()
{
	s_Ptr = nullptr;
}

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	m_Graphics = std::make_unique<Graphics>();
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
				this->m_Graphics->Camera.GetTransform().rotate((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0.0f);
			}
		}
	}

	this->m_Graphics->gameObject.GetTransform().rotate(0.0f, 100 * dt, 0.0f);

	//1인칭 형식 카메라 이동 조작
	float Camera3DSpeed = 0.0001f;

	if (keyboard.KeyIsPressed(VK_SHIFT)) {
		Camera3DSpeed = 0.3f;
	}

	if (keyboard.KeyIsPressed('W')) {
		this->m_Graphics->Camera.GetTransform().translate(this->m_Graphics->Camera.GetTransform().GetForwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S')) {
		this->m_Graphics->Camera.GetTransform().translate(this->m_Graphics->Camera.GetTransform().GetBackwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A')) {
		this->m_Graphics->Camera.GetTransform().translate(this->m_Graphics->Camera.GetTransform().GetLeftVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D')) {
		this->m_Graphics->Camera.GetTransform().translate(this->m_Graphics->Camera.GetTransform().GetRightVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_SPACE)) {
		this->m_Graphics->Camera.GetTransform().translate(0.0f, Camera3DSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z')) {
		this->m_Graphics->Camera.GetTransform().translate(0.0f, -Camera3DSpeed * dt, 0.0f);
	}

	if (keyboard.KeyIsPressed('C')) {
		XMVECTOR lightPosition = this->m_Graphics->Camera.GetTransform().GetPositionVector();
		lightPosition += this->m_Graphics->Camera.GetTransform().GetForwardVector();
		this->m_Graphics->light.GetTransform().SetPosition(lightPosition);
		this->m_Graphics->light.GetTransform().SetRotation(this->m_Graphics->Camera.GetTransform().GetRotationFloat3());
	}
	this->m_Graphics->Camera.UpdateViewMatrix();
}

void Engine::RenderFrame()
{
	m_Graphics->RenderFrame();
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

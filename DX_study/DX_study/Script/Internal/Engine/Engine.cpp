#include "Engine.h"

#include "Timer.h"
#include "../Graphics/Graphics.h"
#include "DeviceResources.h"

Engine* Engine::s_Ptr = nullptr;

Engine & Engine::GetInstance()
{
	return *s_Ptr;
}

Engine::Engine() :
	m_Timer(new Timer),
	m_Graphics(new Graphics),
	m_DeviceResources(&DeviceResources::GetInstance())
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
	
	if (!m_DeviceResources->Initialize(render_window.GetHWND(), width, height)) {
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
				this->m_Graphics->Camera3D.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0.0f);
			}
		}
	}

	this->m_Graphics->gameObject.AdjustRotation(0.0f, dt, 0.0f);

	//1인칭 형식 카메라 이동 조작
	float Camera3DSpeed = 6.0f;

	if (keyboard.KeyIsPressed(VK_SHIFT)) {
		Camera3DSpeed = 30.0f;
	}

	if (keyboard.KeyIsPressed('W')) {
		this->m_Graphics->Camera3D.AdjustPosition(this->m_Graphics->Camera3D.GetForwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S')) {
		this->m_Graphics->Camera3D.AdjustPosition(this->m_Graphics->Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A')) {
		this->m_Graphics->Camera3D.AdjustPosition(this->m_Graphics->Camera3D.GetLeftVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D')) {
		this->m_Graphics->Camera3D.AdjustPosition(this->m_Graphics->Camera3D.GetRightVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_SPACE)) {
		this->m_Graphics->Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z')) {
		this->m_Graphics->Camera3D.AdjustPosition(0.0f, -Camera3DSpeed * dt, 0.0f);
	}

	if (keyboard.KeyIsPressed('C')) {
		XMVECTOR lightPosition = this->m_Graphics->Camera3D.GetPositionVector();
		lightPosition += this->m_Graphics->Camera3D.GetForwardVector();
		this->m_Graphics->light.SetPosition(lightPosition);
		this->m_Graphics->light.SetRotation(this->m_Graphics->Camera3D.GetRotationFloat3());
	}
}

void Engine::RenderFrame()
{
	m_Graphics->RenderFrame();
}

Timer & Engine::GetTimer()
{
	return *(m_Timer.get());
}

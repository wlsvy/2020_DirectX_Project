#include "Engine.h"
#include <functional>

#include "Timer.h"
#include "../Graphics/Graphics.h"
#include "DeviceResources.h"
#include "../Core/ObjectPool.h"
#include "../Core/Scene.h"
#include "../Core/InternalHelper.h"
#include "../../Component/Transform.h"
#include "../Core/GameObject.h"
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
	float Camera3DSpeed = 8.0f * dt;

	if (keyboard.KeyIsPressed(VK_SHIFT)) {
		Camera3DSpeed = 30.0f * dt;
	}

	auto& tf = this->m_Graphics->mainCam->GetTransform();
	if (keyboard.KeyIsPressed('W')) {
		tf.translate(tf.GetForwardVector() * Camera3DSpeed);
	}
	if (keyboard.KeyIsPressed('S')) {
		tf.translate(tf.GetBackwardVector() * Camera3DSpeed);
	}
	if (keyboard.KeyIsPressed('A')) {
		tf.translate(tf.GetLeftVector() * Camera3DSpeed);
	}
	if (keyboard.KeyIsPressed('D')) {
		tf.translate(tf.GetRightVector() * Camera3DSpeed);
	}
	if (keyboard.KeyIsPressed(VK_SPACE)) {
		tf.translate(0.0f, Camera3DSpeed, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z')) {
		tf.translate(0.0f, -Camera3DSpeed, 0.0f);
	}

	if (keyboard.KeyIsPressed('C')) {
		XMVECTOR lightPosition = tf.GetPositionVector();
		lightPosition += tf.GetForwardVector();
		m_Graphics->light->GetTransform().SetPosition(lightPosition);
		m_Graphics->light->GetTransform().SetRotation(tf.GetRotationFloat3());
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
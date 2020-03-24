#include <DirectXMath.h>

#include "Engine.h"
<<<<<<< HEAD:DX_study/DX_study/Engine/Engine.cpp
#include "../Timer.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "AnimationManager.h"
#include "ScriptBehaviourManager.h"
#include "../Component/ScriptBehaviour.h"
=======
#include "Component/ScriptBehaviour.h"
#include <DirectXMath.h>
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/Engine.cpp

using DirectX::operator+=;
using DirectX::operator*;

Engine* Engine::s_Instance = nullptr;

Engine::Engine() :
<<<<<<< HEAD:DX_study/DX_study/Engine/Engine.cpp
	m_Timer(new Timer()),
	m_GraphicsManager(new GraphicsManager()),
	m_ScriptBehaviourManager(new ScriptBehaviourManager()),
	m_SceneManager(new SceneManager()),
	m_PhysicsManager(new PhysicsModule()),
	m_AnimationManager(new AnimationManager())
{}

bool Engine::Initialize(
	HINSTANCE hInstance, 
	std::string window_title, 
	std::string window_class, 
	int width, 
	int height) 
=======
	sceneManager(this, &timer.Time, &physicsManager),
	physicsManager(this, &timer.Time, &physicsBuffer2),
	animationManager(this, &timer.Time, &animatorBuffer),
	scriptBehaviourManager(this, &timer.Time, &scriptBuffer, &keyboard, &mouse, &physicsManager, &mKeyboardEvent, &mMouseEvent)
{
	Engine::s_Instance = this;
}

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) 
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/Engine.cpp
{
	//������ �ʱ�ȭ
	if (!this->m_RenderWindow.Initialize(this, hInstance, window_title, window_class, width, height)) {
		return false;
	}
		

	//�׷��Ƚ� �ʱ�ȭ
	if (!m_GraphicsManager->Initialize(this->m_RenderWindow.GetHWND(), width, height)) {
		return false;
	}
		
	m_GraphicsManager->InitializeSimpleGeometry(m_SceneManager->modelBuffer);
	m_GraphicsManager->InitializeModel(m_SceneManager->modelBuffer);
	
	//�� �Ŵ��� �ʱ�ȭ
	m_SceneManager->Custom_Test_Obj_Set(); // �׽�Ʈ��
	m_PhysicsManager->Initialize();

	m_GraphicsManager->gameObjBuffer = &m_SceneManager->m_GameObjects;
	m_GraphicsManager->InitializeTerrain(m_SceneManager->terrainBuffer);
	
	

	//�׷��Ƚ� �� �ʱ�ȭ
	if (!m_GraphicsManager->InitializeScene()) {
		MessageBoxA(NULL, "Initialize Scene Error", "Error", MB_ICONERROR);
		return false;
	}

	m_ScriptBehaviourManager->Start();

	m_Timer->Start();
	return true;
}

bool Engine::ProcessMessage() {
	return m_QuitFlag ? false : this->m_RenderWindow.ProcessMessage();
}

void Engine::Update() {
<<<<<<< HEAD:DX_study/DX_study/Engine/Engine.cpp
	m_Timer->Tick();
	float dt = Timer::GetDeltaTime();
	m_FixedTimeCheck -= dt;
=======
	timer.Tick();
	float dt = timer.Time.GetDeltaTime();
	fixedTimeCheck -= dt;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/Engine.cpp
	//physicsManager.Update();
	//fixedupdate
	if (m_FixedTimeCheck < 0.0f) { 
		//physicsManager.PhysicsUpdate();
		m_FixedTimeCheck += (float)0.02;
	}
	else {
		//physicsManager.Update();
	}
	m_PhysicsManager->PhysicsUpdate();

	int gameObjSize = m_SceneManager->m_GameObjects.size();
	for (int i = 0; i < gameObjSize; i++) {
		m_SceneManager->m_GameObjects[i]->transform.TRANSFORM_UPDATED = false;
	}

	m_AnimationManager->Update();
	m_ScriptBehaviourManager->Update();
	m_SceneManager->Update();

#pragma region Input Event
	//Ű����, ���콺 �Է°� ����
	m_Keyboard.Update();

	while (!m_Mouse.EventBufferIsEmpty()) {
		MouseEvent me = m_Mouse.ReadEvent();
		if (m_Mouse.IsRightDown()) {//���콺 ������ ��ư ������ �� ȸ��
			if (me.GetType() == MouseEvent::EventType::Raw_MOVE) {
				m_GraphicsManager->Camera3D.AdjustRotation((float)me.GetPosY() * 1.0f * dt, (float)me.GetPosX() * 1.0f * dt, 0.0f);
			}
		}
	}
	//1��Ī ���� ī�޶� �̵� ����
	float Camera3DSpeed = 6.0f;

	if (m_Keyboard.KeyIsPressed(VK_SHIFT)) {
		Camera3DSpeed = 30.0f;
	}

	if (m_Keyboard.KeyIsPressed('W')) {
		m_GraphicsManager->Camera3D.AdjustPosition(m_GraphicsManager->Camera3D.GetForwardVector() * Camera3DSpeed * dt);
	}
	if (m_Keyboard.KeyIsPressed('S')) {
		m_GraphicsManager->Camera3D.AdjustPosition(m_GraphicsManager->Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
	}
	if (m_Keyboard.KeyIsPressed('A')) {
		m_GraphicsManager->Camera3D.AdjustPosition(m_GraphicsManager->Camera3D.GetLeftVector() * Camera3DSpeed * dt);
	}
	if (m_Keyboard.KeyIsPressed('D')) {
		m_GraphicsManager->Camera3D.AdjustPosition(m_GraphicsManager->Camera3D.GetRightVector() * Camera3DSpeed * dt);
	}
	if (m_Keyboard.KeyIsPressed(VK_SPACE)) {
		m_GraphicsManager->Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
	}
	if (m_Keyboard.KeyIsPressed('Z')) {
		m_GraphicsManager->Camera3D.AdjustPosition(0.0f, -Camera3DSpeed * dt, 0.0f);
	}

	if (m_Keyboard.KeyIsPressed('C')) {
		DirectX::XMVECTOR lightPosition = m_GraphicsManager->Camera3D.GetPositionVector();
		lightPosition += m_GraphicsManager->Camera3D.GetForwardVector();
		m_GraphicsManager->light.SetPosition(lightPosition);
		m_GraphicsManager->light.SetRotation(m_GraphicsManager->Camera3D.GetRotationFloat3());
	}
	static int ttttt = 0;
	if (m_Keyboard.KeyIsPressed('P') && ttttt == 0) {
		/*delete scriptBuffer[1];
		ttttt = 1;
		sceneManager.gameObjectBuffer[0]->componentBuffer.erase(sceneManager.gameObjectBuffer[0]->componentBuffer.begin() + 1);
		scriptBuffer.erase(scriptBuffer.begin() + 1);*/
		m_QuitFlag = true;
	}
#pragma endregion

}

void Engine::RenderFrame()
{
	
	m_GraphicsManager->RenderFrame();
	m_GraphicsManager->DrawSkyBox();
	//m_GraphicsManager->DebugDrawTest();
	//m_GraphicsManager->RenderCollider_v2Debug(&physicsBuffer2);
	m_GraphicsManager->ProcessUI();
	m_GraphicsManager->SwapBuffer();
}



GraphicsManager & Engine::GetGraphicsModule()
{
	return *(m_GraphicsManager.get());
}

PhysicsModule & Engine::GetPhysicsManager()
{
	return *(m_PhysicsManager.get());
}

SceneManager& Engine::GetSceneManager()
{
	return *(m_SceneManager.get());
}

ScriptBehaviourManager & Engine::GetScriptBehaviourManager()
{
	return *(m_ScriptBehaviourManager.get());
}

AnimationManager & Engine::GetAnimationManager()
{
	return *(m_AnimationManager.get());
}

KeyboardClass & Engine::GetKeyboard()
{
	return m_Keyboard;
}

MouseClass & Engine::GetMouse()
{
	return m_Mouse;
}

Timer & Engine::GetTimer()
{
	return *(m_Timer.get());
}

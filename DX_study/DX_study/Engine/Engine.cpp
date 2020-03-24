#include <DirectXMath.h>

#include "Engine.h"
#include "../Timer.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "AnimationManager.h"
#include "ScriptBehaviourManager.h"
#include "../Component/ScriptBehaviour.h"

using DirectX::operator+=;
using DirectX::operator*;

//Engine* Engine::s_Instance = nullptr;

Engine::Engine() :
	m_Timer(new Timer()),
	m_GraphicsManager(new GraphicsManager()),
	m_ScriptBehaviourManager(new ScriptBehaviourManager()),
	m_SceneManager(new SceneManager()),
	m_PhysicsManager(new PhysicsModule()),
	m_AnimationManager(new AnimationManager())
{
	//std::shared_ptr<PhysicsModule>(new PhysicsModule);
	//m_PhysicsManager = std::shared_ptr<PhysicsModule>(new PhysicsModule);
}

bool Engine::Initialize(
	HINSTANCE hInstance, 
	std::string window_title, 
	std::string window_class, 
	int width, 
	int height) 
{
	//윈도우 초기화
	if (!this->m_RenderWindow.Initialize(this, hInstance, window_title, window_class, width, height)) {
		return false;
	}
		

	//그래픽스 초기화
	if (!m_GraphicsManager->Initialize(this->m_RenderWindow.GetHWND(), width, height)) {
		return false;
	}
		
	m_GraphicsManager->InitializeSimpleGeometry(m_SceneManager->modelBuffer);
	m_GraphicsManager->InitializeModel(m_SceneManager->modelBuffer);
	
	//씬 매니저 초기화
	m_SceneManager->Custom_Test_Obj_Set(); // 테스트용
	m_PhysicsManager->Initialize();

	m_GraphicsManager->gameObjBuffer = &m_SceneManager->m_GameObjects;
	m_GraphicsManager->InitializeTerrain(m_SceneManager->terrainBuffer);
	
	

	//그래픽스 씬 초기화
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
	m_Timer->Tick();
	float dt = Timer::GetDeltaTime();
	m_FixedTimeCheck -= dt;
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
	//키보드, 마우스 입력값 정보
	m_Keyboard.Update();

	while (!m_Mouse.EventBufferIsEmpty()) {
		MouseEvent me = m_Mouse.ReadEvent();
		if (m_Mouse.IsRightDown()) {//마우스 오른쪽 버튼 눌렀을 때 회전
			if (me.GetType() == MouseEvent::EventType::Raw_MOVE) {
				m_GraphicsManager->Camera3D.AdjustRotation((float)me.GetPosY() * 1.0f * dt, (float)me.GetPosX() * 1.0f * dt, 0.0f);
			}
		}
	}
	//1인칭 형식 카메라 이동 조작
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

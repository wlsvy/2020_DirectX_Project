#include "Engine.h"
#include "../Component/ScriptBehaviour.h"
#include <DirectXMath.h>

using DirectX::operator+=;
using DirectX::operator*;

//Engine* Engine::s_Instance = nullptr;

Engine::Engine() :
	sceneManager(&physicsManager),
	physicsManager(&physicsBuffer2),
	animationManager(&animatorBuffer),
	scriptBehaviourManager(&scriptBuffer, &keyboard, &mouse, &physicsManager, &mKeyboardEvent, &mMouseEvent) {}

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) 
{
	//윈도우 초기화
	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height)) {
		return false;
	}
		

	//그래픽스 초기화
	if (!GraphicsManager::GetInstance().Initialize(this->render_window.GetHWND(), width, height)) {
		return false;
	}
		
	GraphicsManager::GetInstance().InitializeSimpleGeometry(sceneManager.modelBuffer);
	GraphicsManager::GetInstance().InitializeModel(sceneManager.modelBuffer);
	
	//씬 매니저 초기화
	sceneManager.Custom_Test_Obj_Set(); // 테스트용
	physicsManager.Initialize();

	GraphicsManager::GetInstance().gameObjBuffer = &sceneManager.gameObjectBuffer;
	GraphicsManager::GetInstance().lightBuffer = &lightBuffer;
	GraphicsManager::GetInstance().terrainBuffer = &terrainBuffer;
	GraphicsManager::GetInstance().InitializeTerrain(sceneManager.terrainBuffer);
	
	

	//그래픽스 씬 초기화
	if (!GraphicsManager::GetInstance().InitializeScene()) {
		MessageBoxA(NULL, "Initialize Scene Error", "Error", MB_ICONERROR);
		return false;
	}

	scriptBehaviourManager.Start();

	timer.Start();
	return true;
}

bool Engine::ProcessMessage() {
	return ESC_signal ? false : this->render_window.ProcessMessage();
}

void Engine::Update() {
	timer.Tick();
	float dt = Timer::GetDeltaTime();
	fixedTimeCheck -= dt;
	//physicsManager.Update();
	//fixedupdate
	if (fixedTimeCheck < 0.0f) { 
		//physicsManager.PhysicsUpdate();
		fixedTimeCheck += (float)0.02;
	}
	else {
		//physicsManager.Update();
	}
	physicsManager.PhysicsUpdate();

	int gameObjSize = sceneManager.gameObjectBuffer.size();
	for (int i = 0; i < gameObjSize; i++) {
		sceneManager.gameObjectBuffer[i]->transform.TRANSFORM_UPDATED = false;
	}

	animationManager.Update();
	scriptBehaviourManager.Update();
	sceneManager.Update();

#pragma region Input Event
	//키보드, 마우스 입력값 정보
	keyboard.Update();

	while (!mouse.EventBufferIsEmpty()) {
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown()) {//마우스 오른쪽 버튼 눌렀을 때 회전
			if (me.GetType() == MouseEvent::EventType::Raw_MOVE) {
				GraphicsManager::GetInstance().Camera3D.AdjustRotation((float)me.GetPosY() * 1.0f * dt, (float)me.GetPosX() * 1.0f * dt, 0.0f);
			}
		}
	}
	//1인칭 형식 카메라 이동 조작
	float Camera3DSpeed = 6.0f;

	if (keyboard.KeyIsPressed(VK_SHIFT)) {
		Camera3DSpeed = 30.0f;
	}

	if (keyboard.KeyIsPressed('W')) {
		GraphicsManager::GetInstance().Camera3D.AdjustPosition(GraphicsManager::GetInstance().Camera3D.GetForwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S')) {
		GraphicsManager::GetInstance().Camera3D.AdjustPosition(GraphicsManager::GetInstance().Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A')) {
		GraphicsManager::GetInstance().Camera3D.AdjustPosition(GraphicsManager::GetInstance().Camera3D.GetLeftVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D')) {
		GraphicsManager::GetInstance().Camera3D.AdjustPosition(GraphicsManager::GetInstance().Camera3D.GetRightVector() * Camera3DSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_SPACE)) {
		GraphicsManager::GetInstance().Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z')) {
		GraphicsManager::GetInstance().Camera3D.AdjustPosition(0.0f, -Camera3DSpeed * dt, 0.0f);
	}

	if (keyboard.KeyIsPressed('C')) {
		DirectX::XMVECTOR lightPosition = GraphicsManager::GetInstance().Camera3D.GetPositionVector();
		lightPosition += GraphicsManager::GetInstance().Camera3D.GetForwardVector();
		GraphicsManager::GetInstance().light.SetPosition(lightPosition);
		GraphicsManager::GetInstance().light.SetRotation(GraphicsManager::GetInstance().Camera3D.GetRotationFloat3());
	}
	static int ttttt = 0;
	if (keyboard.KeyIsPressed('P') && ttttt == 0) {
		/*delete scriptBuffer[1];
		ttttt = 1;
		sceneManager.gameObjectBuffer[0]->componentBuffer.erase(sceneManager.gameObjectBuffer[0]->componentBuffer.begin() + 1);
		scriptBuffer.erase(scriptBuffer.begin() + 1);*/
		ESC_signal = true;
	}
#pragma endregion

}

void Engine::RenderFrame()
{
	
	GraphicsManager::GetInstance().RenderFrame();
	GraphicsManager::GetInstance().DrawSkyBox();
	//GraphicsManager::GetInstance().DebugDrawTest();
	GraphicsManager::GetInstance().RenderCollider_v2Debug(&physicsBuffer2);
	GraphicsManager::GetInstance().ProcessUI();
	GraphicsManager::GetInstance().SwapBuffer();
}

void Engine::InsertScriptComponent(ScriptBehaviour * _component, GameObject_v2 *_destination)
{
	assert("component is null ptr & add component." && _component != nullptr);
	scriptBuffer.push_back(std::shared_ptr<ScriptBehaviour>(_component));
	_destination->componentBuffer.push_back(std::shared_ptr<Component>(scriptBuffer[scriptBuffer.size() - 1]));
}

void Engine::InsertLightComponent(Light_ver2 * _component, GameObject_v2 *_destination)
{
	assert("component is null ptr & add component." && _component != nullptr);
	lightBuffer.push_back(std::shared_ptr<Light_ver2>(_component));
	_destination->componentBuffer.push_back(std::shared_ptr<Component>(lightBuffer[lightBuffer.size() - 1]));
}

void Engine::InsertTerrainComponent(Terrain * _component, GameObject_v2 *_destination)
{
	assert("component is null ptr & add component." && _component != nullptr);
	terrainBuffer.push_back(std::shared_ptr<Terrain>(_component));
	_destination->componentBuffer.push_back(std::shared_ptr<Component>(terrainBuffer[terrainBuffer.size() - 1]));
}

void Engine::InsertCollider_v2Component(Collider_v2 * _component, GameObject_v2 * _destination)
{
	assert("component is null ptr & add component." && _component != nullptr);
	physicsBuffer2.push_back(std::shared_ptr<Collider_v2>(_component));
	_destination->componentBuffer.push_back(std::shared_ptr<Component>(physicsBuffer2[physicsBuffer2.size() - 1]));
}

void Engine::InsertAnimatorComponent(Animator * _component, GameObject_v2 *_destination) 
{
	assert("component is null ptr & add component." && _component != nullptr);
	//animatorBuffer.push_back(std::shared_ptr<Animator>(_component));
	animatorBuffer.push_back(std::shared_ptr<Animator>(_component));
	_destination->componentBuffer.push_back(std::shared_ptr<Component>(animatorBuffer[animatorBuffer.size() - 1]));
}

void Engine::Component_Valid_Test()
{
	for (std::vector<std::shared_ptr<ScriptBehaviour>>::iterator it = scriptBuffer.begin(); it != scriptBuffer.end();) {
		if (it->unique()) {
			it = scriptBuffer.erase(it);
			continue;
		}
		it++;
	}
	for (std::vector<std::shared_ptr<Light_ver2>>::iterator it = lightBuffer.begin(); it != lightBuffer.end();) {
		if (it->unique()) {
			it = lightBuffer.erase(it);
			continue;
		}
		it++;
	}
	for (std::vector<std::shared_ptr<Terrain>>::iterator it = terrainBuffer.begin(); it != terrainBuffer.end();) {
		if (it->unique()) {
			it = terrainBuffer.erase(it);
			continue;
		}
		it++;
	}
	for (std::vector<std::shared_ptr<Animator>>::iterator it = animatorBuffer.begin(); it != animatorBuffer.end();) {
		if (it->unique()) {
			it = animatorBuffer.erase(it);
			continue;
		}
		it++;
	}
}

SceneManager * Engine::getSceneManager()
{
	return &sceneManager;
}

std::vector<AnimationClip>* Engine::GetAnimClipBuffer()
{
	return &animationManager.mAnimClipBuffer;
}

Engine::~Engine()
{
}

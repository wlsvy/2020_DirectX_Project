#pragma once
#include <memory>

class Transform;
class Camera;
class GameObject;

class Scene {
public:
	static bool IsInitialized() { return s_IsInitialized; }

	Scene();
	void Initialize();
	void ProcessGameObjectTable();
	void Update();
	void OnGui(const char * option = "op");

	std::shared_ptr<Transform> GetWorldTransform() const { return m_WorldTransform; }
	std::shared_ptr<Camera> GetMainCam() const { return m_MainCam; }
	std::weak_ptr<Transform> GetGuiSelected() const { return m_GuiSelectedObj; }

	static void AwakeGameObject(const std::shared_ptr<GameObject> & obj);

	static const float SCENE_MAX_SIZE;
	static const float SCENE_MIN_SIZE;

private:
	void ProcessGuiHirarchy(std::weak_ptr<Transform> tf);

	static bool s_IsInitialized;

	std::shared_ptr<Transform> m_WorldTransform;
	std::shared_ptr<Camera> m_MainCam;

	std::weak_ptr<Transform> m_GuiSelectedObj;
};
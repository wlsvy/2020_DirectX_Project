#pragma once
#include <memory>

class Transform;
class Camera;
class GameObject;

class Scene {
public:
	Scene();
	void Initialize();
	void Update();
	void OnGui();

	std::shared_ptr<Transform> GetWorldTransform() const { return m_WorldTransform; }
	std::shared_ptr<Camera> GetMainCam() const { return m_MainCam; }
	std::weak_ptr<Transform> GetGuiSelected() const { return m_GuiSelectedObj; }

private:
	void ProcessGuiHirarchy(std::weak_ptr<Transform> tf);

	std::shared_ptr<Transform> m_WorldTransform;
	std::shared_ptr<Camera> m_MainCam;

	std::weak_ptr<Transform> m_GuiSelectedObj;
};
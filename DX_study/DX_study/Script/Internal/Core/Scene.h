#pragma once
#include <memory>

class Transform;
class Camera;

class Scene {
public:
	Scene();
	void Initialize();
	void Update();
	void OnGui();

	std::shared_ptr<Transform> GetWorldTransform() const { return m_WorldTransform; }
	std::shared_ptr<Camera> GetMainCam() const { return m_MainCam; }

private:
	std::shared_ptr<Transform> m_WorldTransform;
	std::shared_ptr<Camera> m_MainCam;

};
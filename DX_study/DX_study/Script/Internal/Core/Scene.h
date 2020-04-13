#pragma once
#include <memory>

class Transform;

class Scene {
public:
	Scene();
	void Update();
	void OnGui();

	std::shared_ptr<Transform> GetWorldTransform();
private:
	std::shared_ptr<Transform> m_WorldTransform;

};
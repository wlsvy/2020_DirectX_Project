#pragma once
#include <vector>
#include <memory>

class Engine; //include 정의 조심
class GameObject_v2;
class Component;
class Animator;
class AnimationClip;

class AnimationManager {
	friend class Engine;
public:
	void RegisterComponent(const std::shared_ptr<Animator>& compo);
	void DeregisterComponent(const std::shared_ptr<Animator>& compo);

	void RegisterAnimClip(const std::shared_ptr<AnimationClip>& clip);

	std::vector<std::shared_ptr<AnimationClip>> m_AnimClipBuffer;
private:
	AnimationManager() {}

	void Update();

	std::vector<std::shared_ptr<Animator>> m_AnimatorBuffer;
	
};
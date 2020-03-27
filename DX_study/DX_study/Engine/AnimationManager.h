#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

class Engine; //include 정의 조심
class GameObject_v2;
class Component;
class Animator;
class AnimationClip;

class AnimationManager {
	friend class Engine;
public:
	~AnimationManager() {}

	void RegisterComponent(const std::shared_ptr<Animator>& compo);
	void DeregisterComponent(const std::shared_ptr<Animator>& compo);

	void RegisterClip(const std::shared_ptr<AnimationClip>& clip);

	std::shared_ptr<AnimationClip> GetClip(const std::string & name);

private:
	AnimationManager() {}

	void Update();

	std::vector<std::shared_ptr<Animator>> m_Animators;
	std::unordered_map<std::string, std::shared_ptr<AnimationClip>> m_Clips;
};
#pragma once
#include <vector>
#include "../AnimationClip.h"

class Engine; //include ���� ����
class GameObject_v2;
class Component;
class Animator;

class AnimationManager {
	friend class Engine;
public:
	void RegisterComponent(const std::shared_ptr<Animator>& compo);
	void DeregisterComponent(const std::shared_ptr<Animator>& compo);
private:
	AnimationManager() {}

	void Update();

	std::vector<std::shared_ptr<Animator>> m_AnimatorBuffer;
	std::vector<AnimationClip> mAnimClipBuffer;
};
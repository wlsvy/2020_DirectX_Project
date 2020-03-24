#pragma once
#include <vector>
#include "../AnimationClip.h"

class Engine; //include 정의 조심
class GameObject_v2;
class Component;
class Animator;
class Animator;

class AnimationManager {
	friend class Engine;
private:
	AnimationManager(std::vector<std::shared_ptr<Animator>> * buffer) :
		mAnimatorBuffer(buffer) {}

	void Update();

	std::vector<std::shared_ptr<Animator>> * const mAnimatorBuffer;
	std::vector<AnimationClip> mAnimClipBuffer;
};
#pragma once
#include <vector>
#include "AnimationClip.h"

class Engine; //include 정의 조심
class GameObject_v2;
class Component;
class Animator;
struct TimeInfo;

class AnimationManager {
	friend class Engine;
private:
	AnimationManager(Engine * const engine_ptr, TimeInfo * const timeInfo, std::vector<std::shared_ptr<Animator>> * buffer) :
		engine(engine_ptr),
		Time(timeInfo),
		mAnimatorBuffer(buffer) {}

	void Update();

	std::vector<std::shared_ptr<Animator>> * const mAnimatorBuffer;
	std::vector<AnimationClip> mAnimClipBuffer;

	Engine * const engine;
	TimeInfo * const Time;
};
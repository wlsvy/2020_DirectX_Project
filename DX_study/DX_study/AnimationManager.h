#pragma once
#include <vector>
<<<<<<< HEAD:DX_study/DX_study/Engine/AnimationManager.h
#include <memory>
=======
#include "AnimationClip.h"
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/AnimationManager.h

class Engine; //include 정의 조심
class GameObject_v2;
class Component;
class Animator;
<<<<<<< HEAD:DX_study/DX_study/Engine/AnimationManager.h
class AnimationClip;
=======
struct TimeInfo;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/AnimationManager.h

class AnimationManager {
	friend class Engine;
public:
	void RegisterComponent(const std::shared_ptr<Animator>& compo);
	void DeregisterComponent(const std::shared_ptr<Animator>& compo);

	void RegisterAnimClip(const std::shared_ptr<AnimationClip>& clip);

	std::vector<std::shared_ptr<AnimationClip>> m_AnimClipBuffer;
private:
<<<<<<< HEAD:DX_study/DX_study/Engine/AnimationManager.h
	AnimationManager() {}

	void Update();

	std::vector<std::shared_ptr<Animator>> m_AnimatorBuffer;
	
=======
	AnimationManager(Engine * const engine_ptr, TimeInfo * const timeInfo, std::vector<std::shared_ptr<Animator>> * buffer) :
		engine(engine_ptr),
		Time(timeInfo),
		mAnimatorBuffer(buffer) {}

	void Update();

	std::vector<std::shared_ptr<Animator>> * const mAnimatorBuffer;
	std::vector<AnimationClip> mAnimClipBuffer;

	Engine * const engine;
	TimeInfo * const Time;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/AnimationManager.h
};
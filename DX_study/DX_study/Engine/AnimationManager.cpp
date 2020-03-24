#include "AnimationManager.h"
#include "../Component/Animator.h"

void AnimationManager::Update()
{
	auto begin = mAnimatorBuffer->begin();
	auto end = mAnimatorBuffer->end();

	for (auto it = begin; it != end; it++) {
		bool gameObjectValid = (*it)->gameObject->enabled;
		bool componentValid = (*it)->enabled;
		bool IsRunning = (*it)->mIsRunning;
		if (!(gameObjectValid && componentValid && IsRunning)) continue;

		(*it)->Update(Timer::GetDeltaTime());
	}
}
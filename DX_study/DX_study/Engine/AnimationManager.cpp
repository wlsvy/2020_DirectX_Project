#include "AnimationManager.h"

#include "../Component/Animator.h"
#include "../AnimationClip.h"

void AnimationManager::RegisterComponent(const std::shared_ptr<Animator>& compo)
{
	m_Animators.push_back(compo);
}

void AnimationManager::DeregisterComponent(const std::shared_ptr<Animator>& compo)
{
	auto iter = std::find(m_Animators.begin(), m_Animators.end(), compo);

	if (iter != m_Animators.end()) {
		m_Animators.erase(iter);
	}
}

void AnimationManager::RegisterClip(const std::shared_ptr<AnimationClip>&  clip)
{
	m_Clips.insert(std::make_pair(clip->Name, clip));
}

std::shared_ptr<AnimationClip> AnimationManager::GetClip(const std::string & name)
{
	auto iter = m_Clips.find(name);
	if (iter != m_Clips.end()) {
		return iter->second;
	}
	return std::shared_ptr<AnimationClip>();
}

void AnimationManager::Update()
{
	for (auto& ptr : m_Animators) {
		if (ptr->enabled &&
			ptr->gameObject->enabled &&
			ptr->m_IsRunning)
		{
			ptr->Update(Timer::GetDeltaTime());
		}

	}
}